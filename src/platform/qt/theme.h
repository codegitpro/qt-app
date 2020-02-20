#pragma once
#include <QColor>

namespace ai
{
    struct theme
    {
		theme() : accent_(0x98, 0x43, 0xd2), medium_accent_(0x51, 0x17, 0x83), dark_accent_(0x25, 0x0a, 0x37), error_(0xed, 0x22, 0x25)
		{
		}
		QColor accent() const { return accent_; }
        QColor medium_accent() const { return medium_accent_; }
        QColor dark_accent() const { return dark_accent_; }
		QColor error() const { return error_; }
    private:
        QColor accent_;
        QColor medium_accent_;
        QColor dark_accent_;
		QColor error_;
    };
}
