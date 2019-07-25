#include <mbgl/text/local_glyph_rasterizer.hpp>
#include <mbgl/util/i18n.hpp>

#include <QtCore/QFile>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <qglobal.h>

namespace mbgl {

class LocalGlyphRasterizer::Impl {
public:
    Impl(optional<std::vector<std::string>> fontFamily_);

    bool isConfigured() const;

    optional<std::vector<std::string>> fontFamily;
    QFont font;
};

LocalGlyphRasterizer::Impl::Impl(optional<std::vector<std::string>> fontFamily_)
    : fontFamily(std::move(fontFamily_)) {
    if (isConfigured()) {
        font.setFamily(QString::fromStdString((*fontFamily)[0]));
        font.setPixelSize(24);
    }
}

bool LocalGlyphRasterizer::Impl::isConfigured() const {
    return fontFamily.operator bool();
}

LocalGlyphRasterizer::LocalGlyphRasterizer(optional<std::vector<std::string>> fontFamily)
    : impl(std::make_unique<Impl>(std::move(fontFamily))) {
}

LocalGlyphRasterizer::~LocalGlyphRasterizer() {
}

bool LocalGlyphRasterizer::canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
    return util::i18n::allowsFixedWidthGlyphGeneration(glyphID) && impl->isConfigured();
}

Glyph LocalGlyphRasterizer::rasterizeGlyph(const FontStack&, GlyphID glyphID) {
    Glyph glyph;
    glyph.id = glyphID;

    QFontMetrics metrics(impl->font);
    Size size(metrics.width(glyphID), metrics.height());

    glyph.metrics.width = size.width;
    glyph.metrics.height = size.height;
    glyph.metrics.left = 3;
    glyph.metrics.top = -8;
    glyph.metrics.advance = metrics.width(glyphID);

    QImage image(QSize(size.width, size.height), QImage::Format_Alpha8);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&image);
    painter.setFont(impl->font);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.drawText(QPointF(0, metrics.ascent()), QString(QChar(glyphID)));

    auto img = std::make_unique<uint8_t[]>(image.byteCount());
    memcpy(img.get(), image.constBits(), image.byteCount());

    glyph.bitmap = AlphaImage { size, std::move(img) };

    return glyph;
}

} // namespace mbgl
