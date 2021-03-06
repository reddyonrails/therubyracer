// Copyright 2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef V8_SMART_POINTER_H_
#define V8_SMART_POINTER_H_

namespace v8 {
namespace internal {


// A 'scoped array pointer' that calls DeleteArray on its pointer when the
// destructor is called.
template<typename T>
class SmartPointer {
 public:

  // Default constructor. Construct an empty scoped pointer.
  inline SmartPointer() : p(NULL) {}


  // Construct a scoped pointer from a plain one.
  explicit inline SmartPointer(T* pointer) : p(pointer) {}


  // Copy constructor removes the pointer from the original to avoid double
  // freeing.
  inline SmartPointer(const SmartPointer<T>& rhs) : p(rhs.p) {
    const_cast<SmartPointer<T>&>(rhs).p = NULL;
  }


  // When the destructor of the scoped pointer is executed the plain pointer
  // is deleted using DeleteArray.  This implies that you must allocate with
  // NewArray.
  inline ~SmartPointer() { if (p) DeleteArray(p); }


  // You can get the underlying pointer out with the * operator.
  inline T* operator*() { return p; }


  // You can use [n] to index as if it was a plain pointer
  inline T& operator[](size_t i) {
    return p[i];
  }

  // We don't have implicit conversion to a T* since that hinders migration:
  // You would not be able to change a method from returning a T* to
  // returning an SmartPointer<T> and then get errors wherever it is used.


  // If you want to take out the plain pointer and don't want it automatically
  // deleted then call Detach().  Afterwards, the smart pointer is empty
  // (NULL).
  inline T* Detach() {
    T* temp = p;
    p = NULL;
    return temp;
  }


  // Assignment requires an empty (NULL) SmartPointer as the receiver.  Like
  // the copy constructor it removes the pointer in the original to avoid
  // double freeing.
  inline SmartPointer& operator=(const SmartPointer<T>& rhs) {
    ASSERT(is_empty());
    T* tmp = rhs.p;  // swap to handle self-assignment
    const_cast<SmartPointer<T>&>(rhs).p = NULL;
    p = tmp;
    return *this;
  }


  inline bool is_empty() {
    return p == NULL;
  }


 private:
  T* p;
};

} }  // namespace v8::internal

#endif  // V8_SMART_POINTER_H_
