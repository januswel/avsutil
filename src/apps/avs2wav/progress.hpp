/*
 * progress.hpp
 *  A declaration and definition of a class to show a progress
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef PROGRESS_HPP
#define PROGRESS_HPP

#include "../../helper/algorithm.hpp"
#include "../../helper/elapsed.hpp"
#include "../../helper/wav.hpp"

template<const unsigned int Channels, const unsigned int Bytes>
class progress
: public util::algorithm::basic_progress<
    format::riff_wav::basic_sample<Channels, Bytes>, uint64_t> {
    private:
        util::time::elapsed elapsed;

    protected:
        void output(std::ostream& out,
                    const base_type& numerator, const base_type& denominator) {
            double percentage =
                static_cast<double>(numerator) / denominator * 100;
            out
                << "\r"
                << numerator << "/" << denominator << " samples"
                << " (" << percentage << "%)"
                << " elapsed " << elapsed() << " sec";
        }

    public:
        progress(   std::ostream& out, const base_type& frequency,
                    const base_type& denominator,
                    const base_type& numerator = 0)
            : basic_progress(out, frequency, denominator, numerator) {}
};

#endif // PROGRESS_HPP

