#pragma once

#include "vector.hpp"

namespace basic
{

class IPoolListener
{
public:
    virtual ~IPoolListener(){}

    virtual void on_realocate() = 0;
};

template <class T>
class Pool
{
    struct Node
    {
        T object;
        size_t index;
        Node* next_free;

        template <class ... Args>
        Node( size_t index, Args ... args )
            :object( args ... )
            ,index( index )
            ,next_free( nullptr )
        {
        }
    };

public:
    using handle = size_t;
    static constexpr handle INVALID_HANDLE = 0xFFFFFFFF;
    static constexpr handle FREE_OBJECT = ((handle)1 << 31);

    static Pool<T>& get()
    {
        static Pool<T> pool;

        return pool;
    }

    void add_listener( IPoolListener* listener )
    {
        m_listeners.push( listener );
    }

    void remove_listener( IPoolListener* listener )
    {
        m_listeners.remove_by_value( listener );
    }

    template <class ... Args>
    handle make(Args ... args)
    {
        handle res = FREE_OBJECT; 

        if( m_first_free )
        {
            Node* first_free = m_first_free;
            if( m_first_free->next_free )
            {
                m_first_free = m_first_free->next_free;
            }

            ASSERT( (res & first_free->index) > 0 );

            first_free->index ^= res;
            res = first_free->index;
        }
        else
        {
            res = m_buffer.get_size();

            Node* prev_address = m_buffer.is_empty() ? nullptr : m_buffer.get_raw();

            m_buffer.emplace( res, args ... );

            if( prev_address != nullptr && prev_address != m_buffer.get_raw() )
            {
                for( size_t i = 0; i < m_buffer.get_size(); ++i )
                {
                    m_listeners.get(i)->on_realocate();
                }
            }
        }

        return res;
    }

    void free( handle h )
    {
        Node& node = m_buffer.get( h );

        node.index |= FREE_OBJECT; 

        if( m_first_free )
        {
            Node* last = get_last_free( m_first_free ); 

            ASSERT( last != nullptr && last->next_free == nullptr );

            last->next_free = &node;
        }
        else
        {
            m_first_free = &node;
        }
    }

    T& get( handle h )
    {
        Node& node = m_buffer.get( h );

        ASSERT( node.index == h && h != INVALID_HANDLE );

        return node.object;
    }

private:
    Pool()
        :m_first_free( nullptr )
        ,m_buffer()
    {}

    Node* get_last_free(Node* n)
    {
        if( n && n->next_free )
        {
           return get_last_free( n->next_free ); 
        }

        return n;
    }

    Node* m_first_free;
    Vector<Node> m_buffer;
    Vector<IPoolListener*> m_listeners;
};

template <class T>
class PoolPtr : public IPoolListener
{
public:
    PoolPtr(const PoolPtr<T>& other)
        :m_handle( other.m_handle )
        ,m_cached_object( other.m_cached_object )
    {
        Pool<T>::get().add_listener( this );
    }

    PoolPtr(PoolPtr<T>&& other)
        :m_handle( other.m_handle )
        ,m_cached_object( other.m_cached_object )
    {
        other.m_handle = Pool<T>::INVALID_HANDLE;
        other.m_cached_object = nullptr;
        Pool<T>::get().add_listener( this );
    }

    PoolPtr()
        :m_handle( Pool<T>::INVALID_HANDLE )
		,m_cached_object(nullptr)
    {
        Pool<T>::get().add_listener( this );
    }

    ~PoolPtr()
    {
        Pool<T>::get().remove_listener( this );
    }

    T* get()
    {
        return m_cached_object;
    }

    const T* get() const
    {
        return m_cached_object;
    }

    T* operator -> () { return m_cached_object; }
    const T* operator -> () const { return m_cached_object; }

    template <class ... Args >
    void allocate( Args ... args )
    {
        m_handle = Pool<T>::get().make( args ... );

        m_cached_object = &Pool<T>::get().get( m_handle );
    }

    void free() { Pool<T>::get().free( m_handle ); }

private:
    void on_realocate() override
    {
        if( m_handle != Pool<T>::INVALID_HANDLE )
        {
            m_cached_object = &Pool<T>::get().get( m_handle ); 
        }
    }

private:
    typename Pool<T>::handle m_handle;
    T* m_cached_object;
};


}
