#pragma once
#include <type_traits>

namespace cxx
{
	// �ͷŶ���ָ�����õ��ڴ�
	template<class Ptr_t> void SafeDelete(Ptr_t& pointer)
	{
		delete pointer;
		pointer = nullptr;
	}

	// �ͷŶ���ָ���������õ��ڴ�
	template<class Ptr_t> void SafeDeleteArray(Ptr_t& pointer)
	{
		delete[] pointer;
		pointer = nullptr;
	}

	// ͨ�����ö����Release�ӿ��ͷŶ���
	template<class Ptr_t> void SafeRelease(Ptr_t& pointer)
	{
		if (pointer != nullptr)
		{
			pointer->Release();
			pointer = nullptr;
		}
	}

	// �����и�ֵ�������������������
	template<class T> void Swap(T& l, T& r)
	{
		T t = l;
		l = r;
		r = t;
	}

	// ������С���ֵ
	template<class T> void MinMax(T& min, T& max)
	{
		if (min > max) Swap(min, max);
	}

	// �ں���������ʧ�ܣ���ɨβ�����İ����������������ͳ�� do-whileд��
	// ʹ�������������������
	//    auto fallback = MakeScopeGuard( [&]{ ... } );
	//    ........
	//    fallback.Dismiss();
	//    return true;
	template<class Lambda_t> class ScopeGuard
	{
		bool mDissmissed = false;
		Lambda_t mCallback;
	public:
		ScopeGuard(Lambda_t&& callback) : mCallback(std::move(callback)) { }

		ScopeGuard(ScopeGuard&& other)
			: mCallback(std::move(other.mCallback))
			, mDissmissed(other.mDissmissed)
		{
			other.mDissmissed = true;
		}

		~ScopeGuard()
		{
			if (!mDissmissed)
			{
				mCallback();
			}
		}

		void Dismiss() { mDissmissed = true; }

		bool IsDismissed() const { return mDissmissed; }
	};

	template <class Lambda_t>
	ScopeGuard<Lambda_t> MakeScopeGuard(Lambda_t&& callback)
	{
		return ScopeGuard<Lambda_t>(std::move(callback));
	}


	class Comparison;
	template<class Operand_t>
	Comparison Compare(const Operand_t& l, const Operand_t& r);

	class Comparison
	{
		template<class, class = std::void_t<>>
		struct HasOperatorLess : std::false_type { };

		template<class T>
		struct HasOperatorLess < T, 
			std::void_t<decltype(std::declval<const T&>() < std::declval<const T&>()) >> : std::true_type{ };

		template<class Operand_t>
		friend Comparison Compare(const Operand_t& l, const Operand_t& r);

		enum { Equal, Less, More } mState = Equal;

		Comparison() = default;

	public:

		template<class Operand_t, class = std::enable_if_t<HasOperatorLess<Operand_t>::value> >
		Comparison& Then(const Operand_t& l, const Operand_t& r)
		{
			if (mState == Equal)
			{
				if (l < r)		mState = Less;
				else if (l > r)	mState = More;
			}
			return *this;
		}

		bool IsLess() const { return mState == Less; }

		bool IsMore() const { return mState == More; }

		bool IsEqual() const { return mState == Equal; }

	};

	template<class Operand_t>
	Comparison Compare(const Operand_t& l, const Operand_t& r)
	{
		return Comparison().Then(l, r);
	}

}