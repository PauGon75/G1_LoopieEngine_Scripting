#pragma once

namespace Loopie {
	class Identificable {
	public:
		Identificable();
		virtual ~Identificable();

		virtual size_t GetTypeID() const = 0;

		template<typename T>
		static size_t GetTypeIDStatic() {
			static const char type;
			return reinterpret_cast<size_t>(&type);
		}

		#define DEFINE_TYPE(Type) \
		static size_t GetTypeIDStatic() { \
			static const char type; \
			return reinterpret_cast<size_t>(&type); \
		} \
		size_t GetTypeID() const override { \
			return GetTypeIDStatic(); \
		} 
	};
}