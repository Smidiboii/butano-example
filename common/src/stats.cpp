#include "stats.h"

#include "btn_core.h"
#include "btn_string.h"
#include "btn_display.h"
#include "btn_sprite_text_generator.h"

stats::stats(btn::sprite_text_generator& text_generator) :
    _text_generator(text_generator)
{
    set_mode(_mode);
}

void stats::set_mode(mode_type mode)
{
    int text_x = 8 - (btn::display::width() / 2);
    int text_height = _text_generator.font().item().shape_size().height() + 4;
    _mode = mode;
    _static_text_sprites.clear();
    _text_sprites.clear();
    _max_cpu_usage = 0;
    _counter = 0;

    switch(mode)
    {

    case mode_type::DISABLED:
        break;

    case mode_type::SIMPLE:
        _text_position = btn::fixed_point(text_x, text_height - (btn::display::height() / 2));
        break;

    case mode_type::DETAILED:
        {
            btn::string_view cpu_label = "CPU: ";
            btn::fixed cpu_label_width = _text_generator.width(cpu_label);
            _text_position = btn::fixed_point(text_x + cpu_label_width, text_height - (btn::display::height() / 2));

            int old_bg_priority = _text_generator.bg_priority();
            _text_generator.set_bg_priority(0);

            btn::string<32> text;
            btn::ostringstream text_stream(text);
            text_stream.append(cpu_label);
            _text_generator.generate(text_x, _text_position.y(), text, _static_text_sprites);

            text.clear();
            text_stream.append("IWR: ");
            text_stream.append(btn::memory::used_static_iwram());
            text_stream.append("B");
            _text_generator.generate(text_x, _text_position.y() + text_height, text, _static_text_sprites);

            text.clear();
            text_stream.append("EWR: ");
            text_stream.append(btn::memory::used_static_ewram());
            text_stream.append("B");
            _text_generator.generate(text_x, _text_position.y() + (text_height * 2), text, _static_text_sprites);

            _text_generator.set_bg_priority(old_bg_priority);
        }
        break;

    default:
        BTN_ERROR("Invalid mode: ", int(mode));
        break;
    }
}

void stats::update()
{
    switch(_mode)
    {

    case mode_type::DISABLED:
        return;

    case mode_type::SIMPLE:
        break;

    case mode_type::DETAILED:
        break;

    default:
        BTN_ERROR("Invalid mode: ", int(_mode));
        break;
    }

    _max_cpu_usage = btn::max(_max_cpu_usage, btn::core::cpu_usage());

    if(! _counter)
    {
        btn::fixed max_cpu_pct = _max_cpu_usage * 100;
        btn::string<32> text;
        btn::ostringstream text_stream(text);

        switch(_mode)
        {

        case mode_type::DISABLED:
            break;

        case mode_type::SIMPLE:
            text_stream.append(max_cpu_pct.unsigned_integer());
            break;

        case mode_type::DETAILED:
            text_stream.append(max_cpu_pct);
            break;

        default:
            BTN_ERROR("Invalid mode: ", int(_mode));
            break;
        }

        int old_bg_priority = _text_generator.bg_priority();
        _text_generator.set_bg_priority(0);
        text_stream.append("%");
        _text_sprites.clear();
        _text_generator.generate(_text_position, text, _text_sprites);
        _text_generator.set_bg_priority(old_bg_priority);

        _max_cpu_usage = 0;
        _counter = 60;
    }

    --_counter;
}
