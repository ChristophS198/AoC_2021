#include <vector>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <type_traits>

// Detects if template is used with shared pointer or not
template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};


template<typename T, typename W>
class SortedQueue
{

public:
    using Queue = std::vector<T>;
    SortedQueue() : m_queue{} {};
    SortedQueue(std::initializer_list<T> list_in) : m_queue{list_in} {};
    T get_and_remove_last_entry();
    void push_back(T &&new_elem);
    bool empty();
    size_t size();

    template<typename U = T>
    typename std::enable_if<!is_shared_ptr<U>::value, void>::type
    sort_entries(bool greater);

    template<typename U = T>
    typename std::enable_if<is_shared_ptr<U>::value, void>::type
    sort_entries(bool greater);

    template<typename U = T>
    typename std::enable_if<!is_shared_ptr<U>::value, void>::type
    update_entry(U entry, W (U::*get_val)(), void (U::*set_val)(W))
    {
        auto& elem = find_element(entry);
        if (elem == m_queue.end())
        {
            m_queue.push_back(entry);
        }
        else
        {
            elem->*set_val(entry.*get_val());
        }
    }

    template<typename U = T>
    typename std::enable_if<is_shared_ptr<U>::value, void>::type
    update_entry(U entry,void (U::element_type::*set_predecessor)(U), U (U::element_type::*get_predecessor)(), W (U::element_type::*get_weight)(), void (U::element_type::*set_weight)(W))
    {
        auto elem = find_element(entry);
        if (elem == m_queue.end())
        {
            m_queue.push_back(entry);
        }
        else
        {
            if ((entry->get_weight()) < (*elem)->get_weight())
            {
                (*elem)->set_weight(entry->get_weight());
                (*elem)->set_predecessor(entry->get_predecessor());
            }
        }
    }

private:
    auto find_element(T pt);

    Queue m_queue;
};




// template<typename T, typename W>
// typename std::enable_if<is_shared_ptr<decltype(std::declval<T>().value)>::value, void>::type
// SortedQueue<T,W>::update_entry(const T &entry, W (T::*get_val)(), void (T::*set_val)(W))
// {
//     auto& elem = find_element(entry);
//     std::is_same<void, decltype(elem->*set_val(entry->*get_val()))>::value;
//     if (elem == m_queue.end())
//     {
//         m_queue.push_back(entry);
//     }
//     else
//     {
//         elem->*set_val(entry.*get_val());
//     }
// }

template<typename T, typename W>
template<typename U>
typename std::enable_if<is_shared_ptr<U>::value, void>::type
SortedQueue<T,W>::sort_entries(bool greater) 
{
    std::sort(m_queue.begin(), m_queue.end(), [](const T &a, const T &b) -> bool 
    {
        return (*b) < (*a);
    });
}

template<typename T, typename W>
template<typename U>
typename std::enable_if<!is_shared_ptr<U>::value, void>::type
SortedQueue<T,W>::sort_entries(bool greater) 
{
    std::sort(m_queue.begin(), m_queue.end(), std::greater<T>());
}

template<typename T, typename W>
void SortedQueue<T,W>::push_back(T &&new_elem)
{
    m_queue.push_back(new_elem);
}

template<typename T, typename W>
T SortedQueue<T,W>::get_and_remove_last_entry()
{
    if (m_queue.empty())
    {
        throw std::invalid_argument("Queue empty! No more entries foudn.");
    }
    auto pt = m_queue.back();
    m_queue.pop_back();
    return pt;
}

template<typename T, typename W>
auto SortedQueue<T,W>::find_element(T pt)
{
    return std::find(m_queue.begin(), m_queue.end(), pt);
}

template<typename T, typename W>
bool SortedQueue<T,W>::empty()
{
    return m_queue.empty();
}

template<typename T, typename W>
size_t SortedQueue<T,W>::size()
{
    return m_queue.size();
}
