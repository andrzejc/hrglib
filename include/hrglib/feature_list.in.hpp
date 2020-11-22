/**
 * @file hrglib/feature_list.hpp
 * @brief Definition of `HRGLIB_FEATURE_LIST()` macro.
 */
#pragma once

#ifndef HRGLIB_FEATURE_LIST
/**
 * @brief Second-order macro used as a canonical source for emitting all dependent feature lists,
 *     including the implementation of `hrglib::feature_name` enum.
 */
#define HRGLIB_FEATURE_LIST(visitor) \
    /*[[[cog
import cog
import yaml

def escape(str):
    return str.replace('\\', '\\\\').replace('"', '\\"')

feat_types = ['string']
with open(FEATURES_YAML) as f:
    features = yaml.load(f, Loader=yaml.SafeLoader)
    for feat in features:
        type = feat.get('type', 'string')
        if type not in feat_types:
            feat_types.append(type)
        cog.outl(f"visitor({feat['name']}, {type}, \"{escape(feat['description'])}\") \\")
    ]]]*/
    //[[[end]]]

#endif
