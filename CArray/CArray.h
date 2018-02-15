#pragma once

#include <iterator>
#include <stdexcept>
#include <assert.h>

template <typename TData>
class CArray;

template <typename TData>
class CArrayIterator : public std::iterator<std::random_access_iterator_tag, TData>
{
public:
  CArrayIterator()
    : m_pos(-1)
    , m_array(nullptr)
  {}

  CArrayIterator(
      CArray<TData>* _array,
      int _pos
    )
    : m_pos(_pos)
    , m_array(_array)
  {}

  CArrayIterator(const CArrayIterator&) = default;
  CArrayIterator& operator=(const CArrayIterator&) = default;
  ~CArrayIterator() = default;

public:
  bool operator==(
      const CArrayIterator& _other
    ) const
  {
    assert(m_array == _other.m_array);
    return m_pos == _other.m_pos;
  }

  bool operator!=(
      const CArrayIterator& _other
    ) const
  {
    assert(m_array == _other.m_array);
    return m_pos != _other.m_pos;
  }

  bool operator<(
      const CArrayIterator& _other
    ) const
  {
    assert(m_array == _other.m_array);
    return m_pos < _other.m_pos;
  }

  TData& operator*()
  {
    return (*m_array)[m_pos];
  }

  TData* operator->() const
  {
    return &(*m_array)[m_pos];
  }

  CArrayIterator& operator++()
  {
    ++m_pos;
    return *this;
  }

  CArrayIterator operator++(int)
  {
    CArrayIterator old(*this);
    ++(*this);
    return old;
  }

  CArrayIterator& operator--()
  {
    --m_pos;
    return *this;
  }

  TData& operator[](
      int _n
    ) const
  {
    auto tmp = *this;
    tmp += _n;
    return *tmp;
  }

  CArrayIterator& operator+=(
      int _n
    )
  {
    m_pos += _n;
    return *this;
  }

  CArrayIterator& operator-=(
      int _n
    )
  {
    return *this += -_n;
  }

  CArrayIterator operator+(
      int _n
    ) const
  {
    return CArrayIterator(m_array, m_pos + _n);
  }

  int operator-(
      const CArrayIterator& _other
    ) const
  {
    return m_pos - _other.m_pos;
  }

  CArrayIterator operator-(
      int _n
    ) const
  {
    return CArrayIterator(m_array, m_pos - _n);
  }

private:
  int m_pos;
  CArray<TData>* m_array;
};

template <typename TData>
class CArray
{
public:
  typedef CArrayIterator<TData> iterator;
  typedef CArrayIterator<const TData> const_iterator;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

public:
  CArray();
  CArray(
      std::initializer_list<TData> _values
    );
  CArray(
      const CArray& _array
    );
  ~CArray();

public:
  void push_back(
      const TData& _value
    );
  void pop_back();
  void insert(
      unsigned int _index,
      const TData& _value
    );
  void erase(
      unsigned int _index
    );
  void clear();
  unsigned int size() const;
  unsigned int capacity() const;
  TData& operator[](
      unsigned int _index
    );

private:
  unsigned int get_new_allocation_size(
      unsigned int _requiredSize
    ) const;
  TData* allocate_memory(
      unsigned int _size
    ) const;
  void release_and_clear_memory(
      TData* _ptr,
      unsigned int _objectsNumber
    ) const;
  void copy_objects_to_data_memory(
      TData* _to,
      TData* _from,
      unsigned int _count
    ) const;
  void extended_copy_data(
      unsigned int _newAllocationSize,
      TData*& _toData,
      unsigned int _toSize,
      TData* _fromData,
      unsigned int _fromSize
    ) const;
  void copy_objects_with_shift_insert(
      int _insert_pos,
      const TData& _value
    );
  void copy_objects_with_shift_insert_to_memory(
      TData* _to_data,
      TData* _from_data,
      int _from_size,
      int _insert_pos,
      const TData& _value
    ) const;

private:
  TData* m_data;
  unsigned int m_size;
  unsigned int m_allocationSize;

};


// -----------------------------------------------------------------------------

template<typename TData>
typename CArray<TData>::iterator CArray<TData>::begin()
{
  return iterator(this, 0);
}

template<typename TData>
typename CArray<TData>::iterator CArray<TData>::end()
{
  return iterator(this, m_size);
}

template<typename TData>
typename CArray<TData>::const_iterator CArray<TData>::begin() const
{
  return iterator(this, 0);
}

template<typename TData>
typename CArray<TData>::const_iterator CArray<TData>::end() const
{
  return iterator(this, m_size);
}

template<typename TData>
CArray<TData>::CArray()
  : m_data(nullptr)
  , m_size(0)
  , m_allocationSize(0)
{}

template<typename TData>
CArray<TData>::CArray(
    std::initializer_list<TData> _values
  )
  : m_data(static_cast<TData*>(malloc(_values.size() * sizeof(TData))))
  , m_size(0)
  , m_allocationSize(_values.size())
{
  for (const TData& value: _values) push_back(value);
}

template<typename TData>
CArray<TData>::CArray(
    const CArray& _array
  )
  : m_data(nullptr)
  , m_size(0)
  , m_allocationSize(0)
{
  extended_copy_data(_array.m_size, m_data, m_size, _array.m_data, _array.m_size);
  m_allocationSize = m_size = _array.m_size;
}

template<typename TData>
CArray<TData>::~CArray()
{
  release_and_clear_memory(m_data, m_size);
}

template<typename TData>
void CArray<TData>::push_back(
    const TData& _value
  )
{
  if (m_size < m_allocationSize)
  {
    new (m_data + m_size) TData(_value);
  }
  else
  {
    unsigned int newAllocationSize = get_new_allocation_size(m_size + 1);
    extended_copy_data(newAllocationSize, m_data, m_size, m_data, m_size);
    new (m_data + m_size) TData(_value);
    m_allocationSize = newAllocationSize;
  }
  ++m_size;
}

template<typename TData>
void CArray<TData>::pop_back()
{
  m_data[--m_size].~TData();
}

template<typename TData>
void CArray<TData>::insert(
    unsigned int _index,
    const TData& _value
  )
{
  if (_index == m_size) push_back(_value);
  else
  {
    if (m_size < m_allocationSize)
    {
      copy_objects_with_shift_insert(_index, _value);
    }
    else
    {
      unsigned int newAllocationSize = get_new_allocation_size(m_size + 1);
      TData* newData = allocate_memory(newAllocationSize);
      copy_objects_with_shift_insert_to_memory(newData, m_data, m_size, _index, _value);
      std::swap(newData, m_data);
      release_and_clear_memory(newData, m_size);
      m_allocationSize = newAllocationSize;
    }
    ++m_size;
  }
}

template<typename TData>
void CArray<TData>::erase(
    unsigned int _index
  )
{
  if (_index == (m_size - 1)) pop_back();
  else
  {
    unsigned int i;
    try
    {
      for (i = _index; i < (m_size - 1); ++i)
      {
        m_data[i].~TData();
        new (m_data + i) TData(m_data[i + 1]);
      }
      m_data[m_size - 1].~TData();
    }
    catch(...)
    {
      for (++i; i < m_size; ++i) m_data[i].~TData();
      m_size = _index;
      throw;
    }
    --m_size;
  }
}

template<typename TData>
void CArray<TData>::clear()
{
  release_and_clear_memory(m_data, m_size);
  m_data = nullptr;
  m_size = m_allocationSize = 0;
}

template<typename TData>
unsigned int CArray<TData>::size() const
{
  return m_size;
}

template<typename TData>
unsigned int CArray<TData>::capacity() const
{
  return m_allocationSize;
}

template<typename TData>
TData& CArray<TData>::operator[](
    unsigned int _index
  )
{
  return m_data[_index];
}

template<typename TData>
unsigned int CArray<TData>::get_new_allocation_size(
    unsigned int _requiredSize
  ) const
{
  unsigned int result;

  if (m_allocationSize == 0) result = (_requiredSize <= 2) ? 2 : _requiredSize;
  else
  {
    result = m_allocationSize;
    do
    {
      if (result * sizeof(TData) < 65536) result *= 2;
      else result *= 1.5;
    } while (result < _requiredSize);
  }

  return result;
}

template<typename TData>
TData* CArray<TData>::allocate_memory(
    unsigned int _size
  ) const
{
  TData* dataBlock = static_cast<TData*>(malloc(_size * sizeof(TData)));
  if (!dataBlock) throw std::bad_alloc();
  return dataBlock;
}

template<typename TData>
void CArray<TData>::release_and_clear_memory(
    TData* _ptr,
    unsigned int _objectsNumber
  ) const
{
  for (unsigned int i = 0; i < _objectsNumber; ++i)
  {
    (_ptr + i)->~TData();
  }
  if (_ptr) free(_ptr);
}

template<typename TData>
void CArray<TData>::copy_objects_to_data_memory(
    TData* _to,
    TData* _from,
    unsigned int _count
  ) const
{
  unsigned int i;
  try
  {
    for (i = 0; i < _count; ++i) new (_to + i) TData(_from[i]);
  }
  catch (...)
  {
    release_and_clear_memory(_to, i);
    throw;
  }
}

template<typename TData>
void CArray<TData>::extended_copy_data(
    unsigned int _newAllocationSize,
    TData*& _toData,
    unsigned int _toSize,
    TData* _fromData,
    unsigned int _fromSize
  ) const
{
  TData* newData = allocate_memory(_newAllocationSize);
  copy_objects_to_data_memory(newData, _fromData, _fromSize);
  std::swap(newData, _toData);
  release_and_clear_memory(newData, _toSize);
}

template<typename TData>
void CArray<TData>::copy_objects_with_shift_insert(
    int _insert_pos,
    const TData& _value
  )
{
  int i = m_size - 1;
  try
  {
    new (m_data + m_size) TData(m_data[i]);
    for (--i; i >= _insert_pos; --i) {
      m_data[i+1].~TData();
      new (m_data + i + 1) TData(m_data[i]);
    }
    i = _insert_pos - 1;
    m_data[_insert_pos].~TData();
    new (m_data + _insert_pos) TData(_value);
  }
  catch (...)
  {
    for (i += 2; i < (m_size + 1); ++i) (m_data + i)->~TData();
    m_size = i + 1;
    throw;
  }
}

template<typename TData>
void CArray<TData>::copy_objects_with_shift_insert_to_memory(
    TData* _to_data,
    TData* _from_data,
    int _from_size,
    int _insert_pos,
    const TData& _value
  ) const
{
  int i;
  try
  {
    for (i = _from_size - 1; i >= _insert_pos; --i)
    {
      new (_to_data + i + 1) TData(_from_data[i]);
    }
    i = _insert_pos - 1;
    new (_to_data + i + 1) TData(_value);
    for (--i; i >= -1; --i) new (_to_data + i + 1) TData(_from_data[i + 1]);
  }
  catch (...)
  {
    for (i += 2; i < (m_size + 1); ++i) (_to_data + i)->~TData();
    if (_to_data) free(_to_data);
    throw;
  }
}
