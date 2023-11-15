#ifndef TAGLIB_TAGLIB_EXT_H
#define TAGLIB_TAGLIB_EXT_H

#include "audioproperties.h"
#include "tfile.h"
#include "tiostream.h"

TagLib::File *parse_stream(
        TagLib::IOStream *stream,
        const char *name,
        bool readAudioProperties = false,
        TagLib::AudioProperties::ReadStyle audioPropertiesStyle = TagLib::AudioProperties::Average
);

#endif //TAGLIB_TAGLIB_EXT_H
