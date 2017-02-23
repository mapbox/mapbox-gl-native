#include <mbgl/text/font_store.hpp>
#include <mbgl/text/default_font.hpp>
#include <mbgl/util/thread_local.hpp>

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

#include <mapbox/glyph_foundry.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <mapbox/glyph_foundry_impl.hpp>
#pragma GCC diagnostic pop

#import <CoreGraphics/CoreGraphics.h>
#include <fstream>

typedef struct FontHeader {
    int32_t fVersion;
    uint16_t fNumTables;
    uint16_t fSearchRange;
    uint16_t fEntrySelector;
    uint16_t fRangeShift;
}FontHeader;

typedef struct TableEntry {
    uint32_t fTag;
    uint32_t fCheckSum;
    uint32_t fOffset;
    uint32_t fLength;
}TableEntry;

static uint32_t CalcTableCheckSum(const uint32_t *table, uint32_t numberOfBytesInTable) {
    uint32_t sum = 0;
    uint32_t nLongs = (numberOfBytesInTable + 3) / 4;
    while (nLongs-- > 0) {
        sum += CFSwapInt32HostToBig(*table++);
    }
    return sum;
}


uint32_t fontDataForCGFont (CGFontRef cg_font, void** data) {

    CFRetain(cg_font);
    
    CFArrayRef tags = CGFontCopyTableTags(cg_font);
    int tableCount = CFArrayGetCount(tags);
    
    size_t *tableSizes = (size_t*)malloc(sizeof(size_t) * tableCount);
    memset(tableSizes, 0, sizeof(size_t) * tableCount);
    
    bool containsCFFTable = false;
    
    size_t totalSize = sizeof(FontHeader) + sizeof(TableEntry) * tableCount;
    
    for (int index = 0; index < tableCount; ++index) {
        
        //get size
        size_t tableSize = 0;
        uint32_t aTag = (uint32_t)(uintptr_t)CFArrayGetValueAtIndex(tags, index);
        
        if (aTag == 'CFF ' && !containsCFFTable) {
            containsCFFTable = true;
        }
        
        CFDataRef tableDataRef = CGFontCopyTableForTag(cg_font, aTag);
        if (tableDataRef != NULL) {
            tableSize = CFDataGetLength(tableDataRef);
            CFRelease(tableDataRef);
        }
        totalSize += (tableSize + 3) & ~3;
        
        tableSizes[index] = tableSize;
    }
    
    *data = malloc(totalSize);
    
    memset(*data, 0, totalSize);
    char* dataStart = (char*)*data;
    char* dataPtr = dataStart;
    
    // compute font header entries
    uint16_t entrySelector = 0;
    uint16_t searchRange = 1;
    
    while (searchRange < tableCount >> 1) {
        entrySelector++;
        searchRange <<= 1;
    }
    searchRange <<= 4;
    
    uint16_t rangeShift = (tableCount << 4) - searchRange;
    
    // write font header (also called sfnt header, offset subtable)
    FontHeader* offsetTable = (FontHeader*)dataPtr;
    
    //OpenType Font contains CFF Table use 'OTTO' as version, and with .otf extension
    //otherwise 0001 0000
    offsetTable->fVersion = containsCFFTable ? 'OTTO' : CFSwapInt16HostToBig(1);
    offsetTable->fNumTables = CFSwapInt16HostToBig((uint16_t)tableCount);
    offsetTable->fSearchRange = CFSwapInt16HostToBig((uint16_t)searchRange);
    offsetTable->fEntrySelector = CFSwapInt16HostToBig((uint16_t)entrySelector);
    offsetTable->fRangeShift = CFSwapInt16HostToBig((uint16_t)rangeShift);
    
    dataPtr += sizeof(FontHeader);
    
    // write tables
    TableEntry* entry = (TableEntry*)dataPtr;
    dataPtr += sizeof(TableEntry) * tableCount;
    
    for (int index = 0; index < tableCount; ++index) {
        
        uint32_t aTag = (uint32_t)(uintptr_t)CFArrayGetValueAtIndex(tags, index);
        CFDataRef tableDataRef = CGFontCopyTableForTag(cg_font, aTag);
        size_t tableSize = CFDataGetLength(tableDataRef);
        
        memcpy(dataPtr, CFDataGetBytePtr(tableDataRef), tableSize);
        
        entry->fTag = CFSwapInt32HostToBig((uint32_t)aTag);
        entry->fCheckSum = CFSwapInt32HostToBig(CalcTableCheckSum((uint32_t *)dataPtr, tableSize));
        
        uint32_t offset = dataPtr - dataStart;
        entry->fOffset = CFSwapInt32HostToBig((uint32_t)offset);
        entry->fLength = CFSwapInt32HostToBig((uint32_t)tableSize);
        dataPtr += (tableSize + 3) & ~3;
        ++entry;
        CFRelease(tableDataRef);
    }
    
    CFRelease(cg_font);
    free(tableSizes);

    return totalSize;
}

FT_Face
ft_coretext_face_create (FT_Library library, CGFontRef cg_font)
{
    void* data;
    uint32_t dataSize = fontDataForCGFont(cg_font, &data);
    
    FT_Face ft_face = NULL;
    FT_Open_Args args;
    memset(&args, 0, sizeof(args));
    args.flags = FT_OPEN_MEMORY;
    args.memory_base = (FT_Byte*)data;
    args.memory_size = dataSize;
    
    FT_Error face_error = FT_Open_Face(library, &args, 0, &ft_face);
    if (face_error) {
        throw std::runtime_error("could not open font");
    }
    
    return ft_face;
}

namespace mbgl {


    static util::ThreadLocal<FT_Library> library;
    static util::ThreadLocal<std::vector<LocalFont>> localFonts; // TODO: Shouldn't really be global...
    
    LocalFont::LocalFont(uint32_t id_, const std::string& path)
        : id(id_), ft_face(0)
    {
        if (!library.get()) {
            FT_Library* library_init = new FT_Library;
            // TODO: Match with FT_Done_FreeType(library);
            FT_Error error = FT_Init_FreeType(library_init);
            if (error) {
                throw std::runtime_error("could not open FreeType library");
            }
            library.set(library_init);
        }
        
        std::ifstream file(path.c_str());
        if (file.good()) {
            FT_Error face_error = FT_New_Face(*library.get(), path.c_str(), 0, &ft_face);
            if (face_error) {
                throw std::runtime_error("could not open font");
            }
        } else {
            CGFontRef system_font = CGFontCreateWithFontName(CFStringCreateWithCString(NULL,path.c_str(),kCFStringEncodingUTF8));
            ft_face = ft_coretext_face_create(*library.get(), system_font);
            CFRelease(system_font);
        }
        
        const double scale_factor = 1.0;
        double size = 24 * scale_factor;
        FT_Set_Char_Size(ft_face,0,(FT_F26Dot6)(size * (1<<6)),0,0);
        
        hb_font = hb_ft_font_create(ft_face, NULL);
    }
    
    LocalFont::~LocalFont() {
        // Not being used yet since LocalFonts are just held in a static ThreadLocal
        /*if (hb_font) {
            hb_font_destroy(hb_font);
        }
        if (ft_face) {
            FT_Done_Face(ft_face);
        }*/
    }

    FontStore::FontStore() {

    }

    bool FontStore::UsingLocalFonts() const {
        return !GetDefaultFontPaths().empty();
    }
    
    
    const std::vector<LocalFont>& FontStore::GetLocalFonts() const {
        if (!localFonts.get()) {
            uint32_t fontID = 0;
            std::vector<LocalFont>* threadLocalFonts = new std::vector<LocalFont>;
            for (auto path : GetDefaultFontPaths()) {
                threadLocalFonts->emplace_back(fontID++, path);
            }
            localFonts.set(threadLocalFonts);
        }
        return *localFonts.get();
    }
    
    FT_Face FontStore::GetFT_Face(uint32_t font_id) const {
        if (!localFonts.get()) {
            throw std::runtime_error("FontStore not initialized");
        }
        const std::vector<LocalFont>& threadLocalFonts = *localFonts.get();
        return threadLocalFonts[font_id].ft_face;
    }
    
    hb_font_t* FontStore::GetHB_Font(uint32_t font_id) const {
        if (!localFonts.get()) {
            throw std::runtime_error("FontStore not initialized");
        }
        const std::vector<LocalFont>& threadLocalFonts = *localFonts.get();
        return threadLocalFonts[font_id].hb_font;
    }

    SDFGlyph getSDFGlyph(FT_Face face, uint32_t glyphID) {
        sdf_glyph_foundry::glyph_info glyph;
        glyph.glyph_index = glyphID;
        sdf_glyph_foundry::RenderSDF(glyph, 24, 3, 0.25, face);

        SDFGlyph result;
        GlyphMetrics metrics;
        result.id = glyphID;
        result.bitmap = glyph.bitmap;

        metrics.width = glyph.width;
        metrics.height = glyph.height;
        metrics.left = glyph.left;
        metrics.top = glyph.top;
        metrics.advance = glyph.advance;

        result.metrics = metrics;
        
        return result;
    }



} // namespace mbgl
