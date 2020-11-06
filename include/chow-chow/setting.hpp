#ifndef D73c738defd4416db6bc61674bd0bbb1
#define D73c738defd4416db6bc61674bd0bbb1

#include <cstdint>
#include <cfloat>

namespace ChowChow {
    class Setting {
    public:
        using setting_t = uint32_t;
        static constexpr setting_t SETTING_MAX = UINT32_MAX;

        Setting(double max);

        constexpr auto max() const { return m; }
        constexpr setting_t val_raw() const { return v; }
        constexpr double val() const
        {
            return (static_cast<double>(val_raw()) / SETTING_MAX) * max();
        }

        void max(double f);
        void val(setting_t n) { v = n; }
        void val(double f);

    private:
        double m = 1.;
        double m_eps = DBL_EPSILON;
        setting_t v = 0;
    };
}

#endif
