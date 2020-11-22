/**
 * @file hrglib/feature_types.hpp
 * @brief Header gathering feature #includes and a definition of list of feature
 *     types used for defining `feature_value` type.
 */
#pragma once
#include <string>
#include <cstddef>
/*[[[cog
import cog
import yaml

feat_types = ['string']
headers = ['<string>']
with open(FEATURES_YAML) as f:
    features = yaml.load(f, Loader=yaml.SafeLoader)
    for feat in features:
        header = feat.get('header', '<string>')
        type = feat.get('type', 'string')
        if header not in headers:
            headers.append(header)
        if type not in feat_types:
            feat_types.append(type)

for header in headers:
    if header not in {'<string>', '<cstddef>'}:
        cog.outl(f"#include {header}")
]]]*/
//[[[end]]]

namespace hrglib {
using std::size_t;
using std::string;
}

#ifndef HRGLIB_FEATURE_TYPE_LIST_DIRECT
#define HRGLIB_FEATURE_TYPE_LIST_DIRECT \
    /*[[[cog
num_types = len(feat_types)
for i in range(num_types):
    type = feat_types[i]
    last = (i == num_types - 1)
    cog.outl(f"{type}{'' if last else ','} \\")
    ]]]*/
    //[[[end]]]

#endif

#ifndef HRGLIB_FEATURE_TYPE_LIST
#define HRGLIB_FEATURE_TYPE_LIST(visitor) \
    /*[[[cog
for type in feat_types:
    cog.outl(f"visitor({type}) \\")
    ]]]*/
    //[[[end]]]

#endif
