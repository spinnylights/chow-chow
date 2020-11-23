#ifndef S1ced45b2e3d4532b474c1fca1127b34
#define S1ced45b2e3d4532b474c1fca1127b34

#include <cstdint>
#include <climits>
#include <vector>

#include "chow-chow/frequency.hpp"
#include "chow-chow/phase_acc.hpp"

namespace ChowChow {
    /**
     * @brief An FM operator.
     *
     * An FM operator (technically a phase modulation operator).
     * Generates a sinusoid by itself, but can also be modulated
     * by another signal.
     */
    class Operator {
    public:
        /**
         * @brief The current state of the output signal.
         *
         */
        double sig() const;

        /**
         * @brief Advance the operator by a sample.
         *
         * Advance the internal state of the operator by a period
         * of one sample. Should ordinarily be called once a
         * frame.
         */
        void advance();

        /**
         * @brief Sets the oscillation frequency.
         *
         * @param rate in Hz.
         */
        void freq(double rate);

        /**
         * @brief Adjusts the frequency by a small offset.
         *
         * This can be useful for chorus-like effects.
         *
         * @param factor in the range of -1.0–1.0 or so. 0 is
         * neutral.
         */
        void freq_offset(double factor);

        /**
         * @brief The rate of vibrato.
         *
         * @param rate in Hz; a range of 0.1–19 or so.
         */
        void vibrato_freq(double rate);

        /**
         * @brief The strength of vibrato.
         *
         * @param amp in the range of 1.0–0.1 or so. 0 turns the
         * vibrato off. It is off by default.
         */
        void vibrato_amp(double amp);

        /**
         * @brief The modulation ratio.
         *
         * A coefficient of the frequency. Increases the spacing
         * of the partials in the carrier if this operator is
         * used as a modulator.
         *
         * @param factor ordinarily in the range of 0.25–20.
         * Higher values may cause aliasing if the operator isn't
         * oversampled sufficiently.
         */
        void ratio(double n);

        /**
         * @brief The modulation index.
         *
         * A coefficient of the output signal. Increases the
         * strength and quantity of partials in the carrier if
         * this operator is used as a modulator.
         *
         * @param factor ordinarily in the range of 0.25–20.
         * Higher values may cause aliasing if the operator isn't
         * oversampled sufficiently.
         */
        void index(double n);

        /**
         * @brief The sample rate.
         *
         * @param rate in Hz.
         */
        void sample_rate(PhaseAcc::phase_t rate);

        /**
         * @brief Add a modulator.
         *
         * Store a reference to a modulator that will be used to
         * modulate the output of this operator.
         *
         * @param op A reference to the modulating operator.
         * @param amp Optionally, the amplitude of the modulating
         * signal.
         */
        void add_modulator(const Operator& op, PhaseAcc::amp_t amp = 1.);

        /**
         * @brief Remove modulators.
         *
         * Clear the list of modulators connected to this
         * operator.
         */
        void clear_modulators();

    private:
        void reset_frq();

        static constexpr PhaseAcc::phase_t DEFAULT_SR = 48000;

        struct Mod {
            const Operator* const mod;
            PhaseAcc::amp_t amp = 1.0;
        };

        Frequency frq = 440.;
        Frequency frq_offset = 0.;
        Frequency rtio = 1;
        PhaseAcc phase = {frq, DEFAULT_SR};
        double ndx = 1.;
        PhaseAcc vibr = {1, DEFAULT_SR};
        std::vector<Mod> mods = {};
        bool vibr_on = false;
    };
}

#endif
