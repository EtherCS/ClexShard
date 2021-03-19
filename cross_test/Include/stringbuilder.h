#include <string>

#ifndef __STRINGBUILDER_H__
#define __STRINGBUILDER_H__

// Subset of http://msdn.microsoft.com/en-us/library/system.text.stringbuilder.aspx
template <typename chr=char>
class StringBuilder {
  typedef std::basic_string<chr> string_t;
  typedef std::list<string_t> container_t; // Reasons not to use vector below.
  typedef typename string_t::size_type size_type; // Reuse the size type in the string.
  container_t m_Data;
  size_type m_totalSize;
  StringBuilder & append(const string_t &src) {
    m_Data.push_back(src);
    m_totalSize += src.size();
    return *this; // allow chaining.
  }
  // No copy constructor, no assignement.
  StringBuilder(const StringBuilder &);
  StringBuilder & operator = (const StringBuilder &);
public:
  StringBuilder(const string_t &src) {
    if (!src.empty()) {
      m_Data.push_back(src);
    }
    m_totalSize = src.size();
  }
  StringBuilder() {
    m_totalSize = 0;
  } 
 
  StringBuilder & Append(const string_t &src) {
    return append(src); // allow chaining.
  }
    // This one lets you add any STL container to the string builder.
  template<class inputIterator>
  StringBuilder & Add(const inputIterator &first, const inputIterator &afterLast) {
    // std::for_each and a lambda look like overkill here.
        // <b>Not</b> using std::copy, since we want to update m_totalSize too.
    for (inputIterator f = first; f != afterLast; ++f) {
      append(*f);
    }
    return *this; // allow chaining.
  }
  StringBuilder & AppendLine(const string_t &src) {
    static chr lineEnd[] { '\n', 0 };
    return append(src).append(lineEnd);
  }
  StringBuilder & AppendLine() {
    static chr lineEnd[] { '\n', 0 };
    return append(lineEnd);
  }
 
  string_t ToString() const {
    string_t result;
    result.reserve(m_totalSize + 1);
    for (auto iter = m_Data.begin(); iter != m_Data.end(); ++iter) {
      result += *iter;
    }
    return result;
  }
 
};

#endif // class StringBuilder