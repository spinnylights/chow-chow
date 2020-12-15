#ifndef b85eb0e66e6d4e50ba011e8cfc9b380d
#define b85eb0e66e6d4e50ba011e8cfc9b380d

#include <cstddef>
#include <array>
#include <vector>
#include <iterator>

#include "chow-chow/operator.hpp"

namespace ChowChow {
    template <std::size_t N>
    class Operators {
    public:
        using ops_col_t = std::array<Operator, N>;

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
         * input of another, or change the strength of an
         * existing connection. If the number of the carrier is
         * equal to or higher than the modulator, a feedback loop
         * is implied. Note that feedback loops require extra
         * operations because the signal from some operators will
         * need to be calculated more than once a period.
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
         * @brief Advance the operators by a sample.
         *
         * Advance the internal state of the operators by a
         * period of one sample. Should ordinarily be called once
         * a frame.
         */
        void advance()
        {
            for (auto& o : ops) {
                o.advance();
            }
        }

        /**
         * @brief Recompute the unravelled order the operators will
         * be measured in.
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
            for (auto& o : ops) {
                o.sample_rate(rate);
            }
        }

        /**
         * @brief Compute the output signal.
         *
         * @param time in seconds.
         */
        double sig()
        {
            compute_sig();

            return sum_output();
        }

        /**
         * @copydoc ChowChow::PhaseAcc::quality()
         */
        void quality(uint_fast8_t q)
        {
            for (auto& o : ops) {
                o.quality(q);
            }
        }

    private:
        ops_col_t ops = {};
        std::array<std::array<double, N>, N> cnctns = {};
        std::array<bool, N> outpts = {};
        std::vector<std::size_t> ord = {};
        std::array<double, N> sigs = {};

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
    };
}

#endif
