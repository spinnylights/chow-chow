#ifndef b85eb0e66e6d4e50ba011e8cfc9b380d
#define b85eb0e66e6d4e50ba011e8cfc9b380d

#include <cstddef>
#include <array>
#include <vector>
#include <iterator>
#include <numeric>

#include "chow-chow/operator.hpp"

namespace ChowChow {
    template <std::size_t N>
    class Operators {
    public:
        using ops_col_t = std::array<Operator, N>;

        Operators()
            : oversamp_win(oversamp)
        {}

        /**
         * @brief Subscripting.
         *
         * @param pos 1-indexed (the lowest operator is 1), in
         * deference to how FM "algorithms" are routinely
         * diagrammed.
         */
        Operator& operator[](std::size_t pos)
        {
            return ops.at(pos - 1);
        }

        /**
         * @brief Get the strength of the connection between two
         * operators.
         *
         * @param from the number of the modulator, 1-indexed.
         *
         * @param to the number of the carrier, 1-indexed.
         *
         */
        double connection(std::size_t from, std::size_t to) const
        {
            return cnctns.at(from - 1).at(to - 1);
        }

        /**
         * @brief Check if an operator will be included in the
         * output signal.
         *
         * @param op 1-indexed.
         */
        bool is_output(std::size_t op) const
        {
            return outpts.at(op - 1);
        }

        /**
         * @brief Get the order the operators will be measured
         * in.
         */
        std::vector<std::size_t>& order()
        {
            return ord;
        }

        /**
         * @brief Check the current output signal of an operator.
         *
         * @param op 1-indexed.
         */
        double check_sig(std::size_t op) const
        {
            return ops.at(op - 1).sig();
        }

        /**
         * @brief Connect a modulator to a carrier.
         *
         * Connect the output of one of the operators to the
         * input of another. If the number of the carrier is
         * equal to or higher than the modulator, a feedback loop
         * is implied. Note that feedback loops require extra
         * operations each frame because the signal from some
         * operators will need to be calculated more than once a
         * period.
         *
         * This is intended to be called infrequently because it
         * recomputes the operator order, which is an expensive
         * operation. If you want to change the strength of an
         * existing connection, use Operators::connect_adj.
         *
         * @param from the number of the operator to use as a
         * modulator, 1-indexed.
         *
         * @param to the number of the operator to use as a
         * carrier, 1-indexed.
         *
         * @param amp optionally, a factor to scale the modulator
         * signal by.
         */
        void connect(std::size_t from, std::size_t to, double amp = 1.)
        {
            connect_adj(from, to, amp);
            reorder();
        }

        /**
         * @brief Change the strength of an existing connection
         * between a modulator and a carrier.
         *
         * This is comparable to Operators::connect, but it does
         * not recompute the operator order, so it should not be
         * used during setup (it will have no apparent effect if
         * Operators::connect has not been used first for the
         * from-to pair in question).
         *
         * @param from the number of the operator to use as a
         * modulator, 1-indexed.
         *
         * @param to the number of the operator to use as a
         * carrier, 1-indexed.
         *
         * @param amp optionally, a factor to scale the modulator
         * signal by.
         */
        void connect_adj(std::size_t from, std::size_t to, double amp = 1.)
        {
            cnctns.at(from - 1).at(to - 1) = amp;
        }

        /**
         * @brief Include or exclude an operator from the output signal.
         *
         * @param op the number of the operator, 1-indexed.
         */
        void output(std::size_t op, bool connect = true)
        {
            outpts.at(op - 1) = connect;
        }

        /**
         * @brief Recompute the unravelled order the operators will
         * be measured in.
         *
         * The connections between the operators form a graph,
         * and the output of some operators depends on the output
         * of operators connected to them. Furthermore, some
         * operators feed back to earlier operators, which
         * requires some operators to be re-measured after this
         * point. As a result, if the graph is changed, it must
         * be traversed and a new order determined in which to
         * take the output of the operators.
         *
         * Under ordinary circumstances you should not need to
         * call this function, because Operators::connect will
         * take care of this. However, if for some reason the
         * overhead of Operators::connect is causing you serious
         * problems, you can manage operator connections
         * exclusively with Operators::connect_adj and call this
         * whenever it makes sense to compute the new order.
         */
        void reorder()
        {
            std::vector<std::size_t> o;

            for (long i = N - 1; i >= 0; --i) {
                o.push_back(i);

                for (long j = N - 1; j >= 0; --j) {
                    if (cnctns.at(i).at(j) > 0.) {
                        if (j >= i) { // feedback
                            const auto cur_sz = o.size();
                            long k = cur_sz - 1;
                            for (; k >= 0 && o.at(k) != j ; --k);
                            for(; k < cur_sz; ++k) {
                                o.push_back(o[k]);
                            }
                        }
                    }
                }
            }

            ord = o;
        }

        /**
         * @brief Set the sample rate.
         *
         * Specify the rate the operators will be sampled at.
         *
         * @param rate in Hz.
         */
        void sample_rate(uint64_t rate)
        {
            sample_r = rate;
            set_sr_ops();
        }

        /**
         * @brief Set the amount of oversampling.
         *
         * Oversampling helps to prevent aliasing problems by
         * sampling the operators at a rate well beyond the
         * Nyquist limit. If you are having issues with aliasing,
         * try increasing this value.
         *
         * @param amt a factor to multiply the sample rate by.
         * 2–4 is generally sufficient if using one of the more
         * accurate sine algorithms.
         */
        void oversampling(std::vector<double>::size_type amt)
        {
            if (amt == 0) {
                amt = 1;
            }

            oversamp = amt;
            oversamp_win.resize(oversamp);
            set_sr_ops();
        }

        /**
         * @brief Get the next output signal.
         *
         * Get the signal from the operators, advancing them by
         * one sample period. This is effectful: repeated calls
         * to this will advance the operators repeatedly, giving
         * different results each time. Should generally be
         * called once a frame.
         */
        double sig()
        {
            if (oversamp == 1) {
                return basic_sig();
            } else {
                return oversamp_sig();
            }
        }

        /**
         * @copydoc ChowChow::PhaseAcc::sine_alg(SineAlg)
         */
        void sine_alg(PhaseAcc::SineAlg alg)
        {
            for (auto& o : ops) {
                o.sine_alg(alg);
            }
        }

        /**
         * @copydoc ChowChow::PhaseAcc::sine_alg(std::size_t)
         */
        void sine_alg(std::size_t acc)
        {
            for (auto& o : ops) {
                o.sine_alg(acc);
            }
        }

    private:
        uint64_t sample_r = Operator::DEFAULT_SR;
        std::vector<double>::size_type oversamp = 1;
        ops_col_t ops = {};
        std::array<std::array<double, N>, N> cnctns = {};
        std::array<bool, N> outpts = {};
        std::vector<std::size_t> ord = {};
        std::array<double, N> sigs = {};
        std::vector<double> oversamp_win;

        void compute_sig()
        {
            sigs = {};

            for (std::size_t i = 0; i < ord.size(); ++i) {
                const auto cur = ord[i];
                double mod = 0.;

                for (std::size_t j = 0; j < N; ++j) {
                    const auto cnctn = cnctns[j][cur];
                    if (cnctn > 0.) {
                        mod += sigs[j] * cnctn;
                    }
                }

                sigs[cur] = ops[cur].sig(mod);
            }
        }

        double sum_output() const
        {
            double outp = 0.;

            for (std::size_t i = 0; i < N; ++i) {
                if (outpts[i]) {
                    outp += sigs[i];
                }
            }

            return outp;
        }

        void set_sr_ops()
        {
            for (auto& o : ops) {
                o.sample_rate(sample_r*oversamp);
            }
        }

        void advance()
        {
            for (auto& o : ops) {
                o.advance();
            }
        }

        double basic_sig()
        {
            compute_sig();

            auto o = sum_output();

            advance();

            return o;
        }

        double oversamp_sig()
        {
            for (auto& w : oversamp_win) {
                w = basic_sig();
            }

            return std::accumulate(oversamp_win.begin(),
                                   oversamp_win.end(),
                                   0.)
                   / oversamp;
        }
    };
}

#endif
