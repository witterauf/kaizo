#pragma once

namespace kaizo {

template <class T> class Rectangle
{
public:
    Rectangle() = default;

    explicit Rectangle(T x, T y, T width, T height)
        : m_left{x}
        , m_top{y}
        , m_width{width}
        , m_height{height}
    {
    }

    auto width() const -> T
    {
        return m_width;
    }

    auto height() const -> T
    {
        return m_height;
    }

    auto left() const -> T
    {
        return m_left;
    }

    auto right() const -> T
    {
        return m_left + m_width;
    }

    auto top() const -> T
    {
        return m_top;
    }

    auto bottom() const -> T
    {
        return m_top + m_height;
    }

    void setLeft(T left)
    {
        m_left = left;
    }

    void setRight(T right)
    {
        if (right > m_left)
        {
            m_width = right - m_left;
        }
        else
        {
            m_width = m_left - right;
            m_left = right;
        }
    }

    void setTop(T top)
    {
        m_top = top;
    }

    void setBottom(T bottom)
    {
        if (bottom > m_top)
        {
            m_height = bottom - m_top;
        }
        else
        {
            m_height = m_top - bottom;
            m_top = bottom;
        }
    }

    auto const hasArea() const
    {
        return m_width > 0 && m_height > 0;
    }

private:
    T m_left{0};
    T m_top{0};
    T m_width{0};
    T m_height{0};
};

using BitmapRegion = Rectangle<size_t>;

} // namespace kaizo