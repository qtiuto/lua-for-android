
#ifndef LUADROID_UNIQUE
#define LUADROID_UNIQUE

#include "functional"
#include "new"
namespace std{
    template <class _Tp>
    struct _LIBCPP_TEMPLATE_VIS default_delete {

        _LIBCPP_INLINE_VISIBILITY void operator()(_Tp* __ptr) const _NOEXCEPT {
            static_assert(sizeof(_Tp) > 0,
                          "default_delete can not delete incomplete type");
            static_assert(!is_void<_Tp>::value,
                          "default_delete can not delete incomplete type");
            delete __ptr;
        }
    };
    template <class _Tp, class _Dp = default_delete<_Tp> >
    class  unique_ptr {
    public:
        typedef _Tp element_type;
        typedef _Dp deleter_type;
        typedef _Tp* pointer;

        static_assert(!is_rvalue_reference<deleter_type>::value,
                      "the specified deleter type cannot be an rvalue reference");

    private:
        std::pair<pointer, deleter_type> __ptr_;
    public:
        unique_ptr():__ptr_(pointer(),_Dp()){}

        unique_ptr(pointer p)noexcept :__ptr_(p,_Dp()){}

        unique_ptr(unique_ptr&& __u) noexcept
                : __ptr_(__u.release(), std::forward<deleter_type>(__u.get_deleter())) {
        }

        template <class _Up, class _Ep>
        unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
                : __ptr_(__u.release(), std::forward<_Ep>(__u.get_deleter())) {}

        unique_ptr& operator=(unique_ptr&& __u) noexcept {
            reset(__u.release());
            __ptr_.second = std::forward<deleter_type>(__u.get_deleter());
            return *this;
        }
        template <class _Up, class _Ep>
        unique_ptr& operator=(unique_ptr<_Up, _Ep>&& __u) noexcept {
            reset(__u.release());
            __ptr_.second = std::forward<_Ep>(__u.get_deleter());
            return *this;
        }



        ~unique_ptr() { reset(); }

        typename add_lvalue_reference<_Tp>::type
        operator*() const {
            return *__ptr_.first;
        }

        pointer operator->() const noexcept {
            return __ptr_.first;
        }

        pointer get() const noexcept {
            return __ptr_.first;
        }

        deleter_type& get_deleter() noexcept {
            return __ptr_.second;
        }

        const deleter_type& get_deleter() const noexcept {
            return __ptr_.second;
        }

        explicit operator bool() const noexcept {
            return __ptr_.first != nullptr;
        }


        pointer release() noexcept {
            pointer __t = __ptr_.first;
            __ptr_.first = pointer();
            return __t;
        }


        void reset(pointer __p = pointer()) noexcept {
            pointer __tmp = __ptr_.first;
            __ptr_.first = __p;
            if (__tmp)
                __ptr_.second(__tmp);
        }
    };
}

#endif