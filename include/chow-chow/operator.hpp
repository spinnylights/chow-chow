#ifndef S1ced45b2e3d4532b474c1fca1127b34
#define S1ced45b2e3d4532b474c1fca1127b34

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
         * @brief The current state of the wave, taking into
         * account the ratio, offset, and vibrato.
         *
         * @param time in seconds.
         */
        double wave(double time) const;

        /**
         * @brief The current state of the vibrato wave.
         *
         * @param time in seconds.
         */
        double vibrato(double time) const;

        /**
         * @brief The current state of the output signal.
         *
         * @param time in seconds.
         * @param mod optionally, the modulating signal.
         */
        double sig(double time, double mod = 0.) const;

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
         * vibrato off.
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

    private:
        double frq = 440.;
        double frq_offset = 0.;
        double rtio = 1.;
        double vibr_freq = 0.;
        double vibr_amp = 0.;
        double ndx = 1.;
    };
}

#endif
