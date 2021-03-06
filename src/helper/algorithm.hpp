/*
 * algorithm.hpp
 *  some functions to comply with the concpet of STL, like <algorithm>
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <algorithm>
#include <iterator>
#include <vector>

namespace util {
    namespace algorithm {
        // functor to delete objects
        // usage:
        //      std::for_each(array.begin(), array.end(), sweeper());
        struct sweeper {
            template<typename T>
            void operator()(T* p) { delete p; }
        };

        // for std::generate() and std::generate_n()
        template<typename T> class counter {
            public:
                typedef T   count_type;

            private:
                count_type first, returned;
                const count_type increment;

            public:
                counter(count_type first, count_type increment)
                    : first(first), increment(increment) {}
                count_type operator()(void) {
                    returned = first;
                    first += increment;
                    return returned;
                }

            private:
                counter& operator=(const counter& rhs);
        };

        // copy_if
        template<   typename InputIterator, typename OstreamIterator,
                    typename Predicate>
            inline OstreamIterator
            copy_if(InputIterator first, InputIterator last,
                    OstreamIterator result, Predicate pred) {
                for (; first != last; ++first)
                    if (pred(*first)) *result++ = *first;
                return result;
            }

        // print
        template<typename InputIterator, typename OstreamIterator>
            inline OstreamIterator
            print(  InputIterator first, InputIterator last,
                    OstreamIterator out) {
                return std::copy(first, last, out);
            }

        // the version that the type of delimiter is const char_type* const
        template<typename Value, typename InputIterator, typename Ostream>
            inline std::ostream_iterator<Value, typename Ostream::char_type>
            print(  InputIterator first, InputIterator last, Ostream& ostream,
                    const typename Ostream::char_type* const delimiter) {
                return print(
                        first, last,
                        std::ostream_iterator<  Value,
                                                typename Ostream::char_type>(
                            ostream, delimiter));
            }

        // print_op with unary oprator
        template<   typename InputIterator, typename OstreamIterator,
                    typename UnaryOperator>
            inline OstreamIterator
            print_op(   InputIterator first, InputIterator last,
                        OstreamIterator out, UnaryOperator op) {
                return std::transform(first, last, out, op);
            }

        // the version that the type of delimiter is const char_type* const
        template<   typename Value, typename InputIterator,
                    typename Ostream, typename UnaryOperator>
            inline std::ostream_iterator<Value, typename Ostream::char_type>
            print_op(   InputIterator first, InputIterator last,
                        Ostream& ostream,
                        const typename Ostream::char_type* const delimiter,
                        UnaryOperator op) {
                return print_op(
                        first, last,
                        std::ostream_iterator<  Value,
                                                typename Ostream::char_type>(
                            ostream, delimiter), op);
            }

        // print_op with binary oprator
        template<   typename InputIterator1, typename InputIterator2,
                    typename OstreamIterator, typename BinaryOperator>
            inline OstreamIterator
            print_op(   InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, OstreamIterator out,
                        BinaryOperator op) {
                return std::transform(first1, last1, first2, out, op);
            }

        // the version that the type of delimiter is const char_type>* const
        template<   typename Value, typename InputIterator1,
                    typename InputIterator2, typename Ostream,
                    typename BinaryOperator>
            inline std::ostream_iterator<Value, typename Ostream::char_type>
            print_op(   InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, Ostream& ostream,
                        const typename Ostream::char_type* const delimiter,
                        BinaryOperator op) {
                return print_op(
                        first1, last1, first2,
                        std::ostream_iterator<  Value,
                                                typename Ostream::char_type>(
                            ostream, delimiter), op);
            }

        // print_if
        template<   typename InputIterator, typename OstreamIterator,
                    typename Predicate>
            inline OstreamIterator
            print_if(   InputIterator first, InputIterator last,
                        OstreamIterator out, Predicate pred) {
                return util::algorithm::copy_if(first, last, out, pred);
            }

        // the version that the type of delimiter is const char_type* const
        template<   typename Value, typename InputIterator,
                    typename Ostream, typename Predicate>
            inline std::ostream_iterator<Value, typename Ostream::char_type>
            print_if(   InputIterator first, InputIterator last,
                        Ostream& ostream,
                        const typename Ostream::char_type* const delimiter,
                        Predicate pred) {
                return print_if(
                        first, last,
                        std::ostream_iterator<  Value,
                                                typename Ostream::char_type>(
                            ostream, delimiter), pred);
            }

        // grep in Perl
        template<   typename InputIterator, typename SequenceContainer,
                    typename Predicate>
            inline SequenceContainer&
            grep(   InputIterator first, InputIterator last,
                    SequenceContainer& result, Predicate pred) {
                result.resize(0);
                for (; first != last; ++first)
                    if (pred(*first)) result.push_back(*first);
                return result;
            }

        template<   typename InputIterator, typename Value,
                    typename Predicate>
            inline std::vector<Value>&
            grep(   InputIterator first, InputIterator last,
                    std::vector<Value>& result, Predicate pred) {
                unsigned int n = std::count_if(first, last, pred);
                result.resize(n);
                result.reserve(n);
                util::algorithm::copy_if(first, last, result.begin(), pred);
                return result;
            }

        /*
         *  A class to show progress
         *  Usage:
         *
         *      std::transform(
         *          longlongstring.begin(), longlongstring.end(),
         *          std::ostream_iterator<char>(std::cout),
         *          basic_progress<char, unsigned int>(
         *              std::cerr, 4096, numof_data));
         * */
        template<typename Result, typename Base>
            class basic_progress {
                public:
                    typedef Result  result_type;
                    typedef Base    base_type;

                protected:
                    std::ostream* out;
                    base_type frequency;
                    base_type denominator;
                    base_type numerator;

                public:
                    basic_progress(
                            std::ostream& out,
                            const base_type& frequency,
                            const base_type& denominator,
                            const base_type& numerator = 0)
                        : out(&out), frequency(frequency),
                          denominator(denominator), numerator(numerator) {}

                    result_type operator()(const result_type& result) {
                        ++numerator;
                        if (       numerator % frequency == 0
                                || numerator == denominator) {
                            output(*out, numerator, denominator);
                        }
                        return result;
                    }

                protected:
                    virtual void output( std::ostream& out,
                            const base_type& numerator,
                            const base_type& denominator) {
                        double percentage =
                            static_cast<double>(numerator) / denominator * 100;
                        out
                            << "\r"
                            << numerator << "/" << denominator << " samples"
                            << " (" << percentage << "%)";
                    }
            };
    }
}

#endif // ALGORITHM_HPP

