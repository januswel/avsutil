/*
 * manager_impl.cpp
 *  Definitions for cmanager_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

namespace avsutil {
    namespace impl {
        avs_type& cmanager_type::load(const char* file_path) {
            DBGLOG("cavs_loader_type::load(" << file_path << ")");

            cavses_type::iterator found =
                std::find_if(
                        cavses.begin(), cavses.end(),
                        std::bind2nd(std::mem_fun(
                                &avsutil::impl::cavs_type::is_me), file_path));

            // found
            if (found != cavses.end()) return **found;

            // not found and create
            cavs_type* created = new impl::cavs_type();
            created->open(file_path);
            cavses.push_back(created);
            return *created;
        }

        void cmanager_type::unload(const avs_type& target) {
            DBGLOG( "cavs_loader_type::release("
                    << target.filepath() << ")");

            cavses_type::iterator found =
                std::find(cavses.begin(), cavses.end(), &target);

            if (found != cavses.end()) {
                delete *found;
                cavses.erase(found);
            }
        }
    }
}

