#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <string>
#include "ns3/enum.h"
namespace ns3 {

class DataType {
	public:
		enum Type : uint8_t {
			FLOAT16
			FLOAT32,
			FLOAT64,
			INT32,
			INT16
		};

		static inline uint32_t GetSizeBytes(Type t){
			switch (t) {
				case FLOAT32: return 4;
				case FLOAT64: return 8;
				case INT32:   return 4;
				case INT16:   return 2;
				default:      return 0;
			}
		}

		static inline const char* ToString(Type t){
			switch (t) {
				case FLOAT32: return "FLOAT32";
				case FLOAT64: return "FLOAT64";
				case INT32:   return "INT32";
				case INT16:   return "INT16";
				default:      return "UNKNOWN";
			}
		}
};

} // namespace ns3
#endif
