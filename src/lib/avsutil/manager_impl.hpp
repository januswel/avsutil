/*
 * manager_impl.hpp
 *  Declarations and definitions for a class cmanager_type
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef MANAGER_IMPL_HPP
#define MANAGER_IMPL_HPP

#include "../../include/avsutil.hpp"

#include "avs_impl.hpp"

#include <list>
#include <algorithm>

#include "../../helper/algorithm.hpp"
#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        class cmanager_type : public manager_type {
            private:
                typedef std::list<cavs_type*> cavses_type;

            private:
                cavses_type cavses;

            public:
                // constructor
                cmanager_type(void) {
                    DBGLOG("cmanager_type::cmanager_type(void)");
                }

            public:
                // destructor
                ~cmanager_type(void) {
                    DBGLOG("cmanager_type::~cmanager_type(void)");
                    std::for_each(
                            cavses.rbegin(), cavses.rend(),
                            util::algorithm::sweeper());
                }

            public:
                // equivalent operator
                bool operator==(const cmanager_type& rhs) {
                    return this == &rhs;
                }
                // non-equivalent operator
                bool operator!=(const cmanager_type& rhs) {
                    return !(*this == rhs);
                }

            private:
                // Inhibits copy and assignment.
                // copy constructor
                explicit cmanager_type(const cmanager_type& rhs);
                // assignment operator
                cmanager_type& operator=(const cmanager_type& rhs);

            public:
                /*
                 *  Implementations of some member functions of a super class
                 *  manager_type.
                 * */
                avs_type& load(const char* file_path) {
                    DBGLOG("cavs_loader_type::load(" << file_path << ")");

                    cavses_type::iterator found =
                        std::find_if(
                                cavses.begin(), cavses.end(),
                                std::bind2nd(
                                    std::mem_fun(
                                        &avsutil::impl::cavs_type::is_me),
                                    file_path));

                    // found
                    if (found != cavses.end()) return **found;

                    // not found and create
                    cavs_type* created = new impl::cavs_type();
                    created->open(file_path);
                    cavses.push_back(created);
                    return *created;
                }

                void unload(const avs_type& target) {
                    DBGLOG( "cavs_loader_type::release("
                            << target.filepath() << ")");

                    cavses_type::iterator found =
                        std::find(cavses.begin(), cavses.end(), &target);

                    if (found != cavses.end()) {
                        delete *found;
                        cavses.erase(found);
                    }
                }
        };
    }
}

#endif // MANAGER_IMPL_HPP

