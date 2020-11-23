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
        Operator& operator[](std::size_t pos) { return ops.at(pos - 1); }

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
         * input of another. If the number of the carrier is equal
         * to or higher than the modulator, a feedback loop is
         * implied.
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
            ops.at(to - 1).add_modulator(ops.at(from - 1), amp);
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
            for (auto o = ops.rbegin(); o != ops.rend(); ++o) {
                o->advance();
            }
        }

        /**
         * @brief Compute the output signal.
         *
         * @param time in seconds.
         */
        double sig()
        {
            double outp = 0.;

            for (std::size_t i = 0; i < N; ++i) {
                if (outpts[i]) {
                    outp += ops[i].sig();
                }
            }

            return outp;
        }

        /**
         * @brief Set the sample rate.
         *
         * Specify the rate the operators will be sampled at.
         *
         * @param rate in Hz.
         */
        void sample_rate(PhaseAcc::phase_t rate)
        {
            for (auto& o : ops) {
                o.sample_rate(rate);
            }
        }

    private:
        ops_col_t ops = {};
        std::array<bool, N> outpts = {};
    };
}

#endif
