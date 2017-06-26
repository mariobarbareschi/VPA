#ifndef VPA_H
#define VPA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <map>

///////////////////////////////////////////////////////////////////////////////
/// Utilities for uin128_t, to enable compile with gcc >= 4.7.0 and -m64 flag
#ifdef __SIZEOF_INT128__

// New types
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;

#define INT128_MASK_BIT_HIGH(pos)     (((uint128_t)0x01) << pos)
#define INT128_MASK_BIT_LOW(pos)      ~(INT128_MASK_BIT_HIGH(pos))
#define INT128_MASK_LOWER_LOW(num)    (((((uint128_t)0xFFFFFFFFFFFFFFFF) << 64) | 0xFFFFFFFFFFFFFFFF) << num)
#define INT128_MASK_LOWER_HIGH(num)   (~INT128_MASK_LOWER_LOW(num))
#define INT128_MASK_HIGHER_HIGH(num)  (INT128_MASK_LOWER_LOW(128 - num))

// I/O functions
#define print_uint128_hex(val) \
  do {\
    printf("%016lx", (uint64_t) (val >> 64));\
    printf("%016lx", (uint64_t) val);\
  } while(0)

#define print_int128_hex(val) print_uint128_hex(val)

inline ::std::ostream& operator<<(::std::ostream& out, uint128_t val) {
  out << ::std::setw(16) << ::std::setfill('0') << ::std::hex
      << (uint64_t) (val >> 64);
  out << ::std::setw(16) << ::std::setfill('0') << ::std::hex << (uint64_t) val
      << ::std::dec;
  return out;
}
inline ::std::ostream& operator<<(::std::ostream& out, int128_t val) {
  out << (uint128_t)val;
  return out;
}
#endif
///////////////////////////////////////////////////////////////////////////////
//#define _FAP_DEBUG_

#define _VPA_LIBRARY_

#define HALF_SIGN_SIZE               1
#define HALF_EXP_SIZE                5
#define HALF_MANT_SIZE               10
#define HALF_SIZE                    HALF_EXP_SIZE + HALF_MANT_SIZE + HALF_SIGN_SIZE

#define FLOAT_SIGN_SIZE               1
#define FLOAT_EXP_SIZE                8
#define FLOAT_MANT_SIZE               23
#define FLOAT_SIZE                    FLOAT_EXP_SIZE + FLOAT_MANT_SIZE + FLOAT_SIGN_SIZE

#define DOUBLE_SIGN_SIZE              1
#define DOUBLE_EXP_SIZE               11
#define DOUBLE_MANT_SIZE              52
#define DOUBLE_SIZE                   DOUBLE_EXP_SIZE + DOUBLE_MANT_SIZE + DOUBLE_SIGN_SIZE

typedef uint8_t SignType;
typedef uint16_t ExpType;
typedef uint128_t MantType;

#define EXPONENT_BIAS(prec)           MASK_LOWER_HIGH(ExpType, (prec - 1))

#define MASK_BIT_HIGH(type, pos)      (type)(INT128_MASK_BIT_HIGH(pos)) ///< Create a mask with the pos-nth bit high
#define MASK_BIT_LOW(type, pos)       (type)((~MASK_BIT_HIGH(type, pos))) ///< Create a mask with the pos-nth bit low where the others are high
#define MASK_LOWER_LOW(type, num)     (type)(INT128_MASK_LOWER_LOW(num)) ///< Create a mask with all lower bit low for num positions
#define MASK_LOWER_HIGH(type, num)    (type)((~MASK_LOWER_LOW(type, num))) ///< Create a mask with all lower bit high for num positions
#define MASK_HIGHER_HIGH(type, num)   (type)(INT128_MASK_HIGHER_HIGH(num)) ///< Create a mask with all lower bit high for num positions

/// @brief Rounding methods
typedef enum {
  VPA_FP_ROUND_TOWARD_0 = 0,
  VPA_FP_ROUND_TOWARD_PINF,
  VPA_FP_ROUND_TOWARD_NINF,
  VPA_FP_ROUND_NEAREST
} VPA_rounding_method;

///@defgroup VPA_FP_SHIFTING_FUNCTIONS VPA Floating Point Shifting functions
/// @{
void vpa_shift_right_(uint128_t*, int to_shift, uint8_t* grs);
void vpa_shift_left_(uint128_t*, int to_shift, uint8_t* grs);
/// @}

namespace vpa {

using IntegerPrecision = uint8_t;


struct FloatPointPrecTy{
    uint16_t exp_size;  ///< Size of the exponent
    uint16_t mant_size;  ///< Size of the mantissa
    FloatPointPrecTy(uint16_t e_size = 0, uint16_t m_size = 0)
      : exp_size(e_size),
        mant_size(m_size) {
  }
};

/*enum FloatPointPrecTy{
    half = FloatPointPrecTy(HALF_EXP_SIZE,FLOAT_MANT_SIZE),
    float = FloatPointPrecTy(DOUBLE_EXP_SIZE,DOUBLE_MANT_SIZE),
    double = FloatPointPrecTy(DOUBLE_EXP_SIZE,DOUBLE_MANT_SIZE)
};*/
    
#define half_prec FloatPointPrecTy(HALF_EXP_SIZE,HALF_MANT_SIZE)
#define float_prec FloatPointPrecTy(FLOAT_EXP_SIZE,FLOAT_MANT_SIZE)
#define double_prec FloatPointPrecTy(DOUBLE_EXP_SIZE,DOUBLE_MANT_SIZE)

class VPA {
    /// @brief Class for Variable Precision Aritmetic
private:
  ::std::string name; ///< For debug purposes
    
  SignType sign;  ///< Sign used 1 bit
  ExpType exp;  ///< Exponent on max 16 bit
  MantType mant;  ///< Mantissa on max 64 bit
  uint8_t grs;  ///< Guard, round and sticky bits of the mantissa
  FloatPointPrecTy prec;  ///< Information about the precision
 public:
  /// \{
  /// \brief Default ctor
  VPA()
      : sign(0),
        exp(0),
        mant(0),
        grs(0),
        prec(FloatPointPrecTy()) {}

  /// @brief Conversion from float
  VPA(float f, FloatPointPrecTy n_prec = float_prec) {
    *this = f;
    this->changePrec(n_prec);
  }

  /// @brief Conversion from double
  VPA(double d, FloatPointPrecTy n_prec = double_prec) {
    *this = d;
    this->changePrec(n_prec);
  }

  /// \{
  // Getters and Setters
  SignType getSign() const {
    return (this->sign & MASK_BIT_HIGH(SignType, 0));
  }

  void setSign(SignType sign) {
    this->sign = (sign & MASK_BIT_HIGH(SignType, 0));
  }

  ExpType getExp() const {
    return (this->exp & MASK_LOWER_HIGH(ExpType, this->prec.exp_size));
  }

  void setExp(ExpType exp) {
    this->exp = (exp & MASK_LOWER_HIGH(ExpType, this->prec.exp_size));
  }

  MantType getMant() const {
    return this->mant;
  }

  void setMant(MantType mant) {
    this->mant = (mant & MASK_LOWER_HIGH(MantType, this->prec.mant_size));
  }

  MantType getMantHb() const {
    // If mant is 0 this is the zero floating point
    if (this->isZero()) {
      return (MantType) 0;
    }
    return (MASK_BIT_HIGH(MantType, this->prec.mant_size) | this->mant);
  }

  void setMantHb() {
    this->mant = this->getMantHb();
  }

  uint8_t getGrs() const {
    return grs;
  }

  void setGrs(uint8_t grs) {
    this->grs = grs;
  }

  FloatPointPrecTy getPrec() const {
    return prec;
  }

  void setPrec(FloatPointPrecTy prec) {
    this->prec = prec;
  }

  const ::std::string& getName() const {
    return name;
  }

  void setName(const ::std::string& name) {
    this->name = name;
  }
  /// \}

  // Overloaded operators
  /// brief Conversion to float
  explicit operator float() const;
  /// brief Conversion to double
  explicit operator double() const;

  // Assign operators
  VPA& operator=(float);
  VPA& operator=(double);

  // Arithmetic operators
  // Each operations is done at the minimum precision between the operands
  VPA& operator+=(const VPA& fp);
  VPA& operator-=(const VPA& fp);
  VPA& operator*=(const VPA& fp);
  VPA& operator/=(const VPA& fp);

  friend VPA operator+(VPA lhs,
                                     const VPA& rhs) {
    lhs += rhs;
    return lhs;
  }
  friend VPA operator-(VPA lhs,
                                     const VPA& rhs) {
    lhs -= rhs;
    return lhs;
  }
  friend VPA operator*(VPA lhs,
                                     const VPA& rhs) {
    lhs *= rhs;
    return lhs;
  }
  friend VPA operator/(VPA lhs,
                                     const VPA& rhs) {
    lhs /= rhs;
    return lhs;
  }

  // Unary Operator
  friend VPA operator-(VPA lhs) {
    lhs.setSign(~lhs.getSign());
    return lhs;
  }

  // Relational operators
  bool operator<=(const VPA& rhs) {
    return static_cast<double>(*this) <= static_cast<double>(rhs);
  }
  bool operator<(const VPA& rhs) {
    return static_cast<double>(*this) < static_cast<double>(rhs);
  }
  bool operator>=(const VPA& rhs) {
    return static_cast<double>(*this) >= static_cast<double>(rhs);
  }
  bool operator>(const VPA& rhs) {
    return static_cast<double>(*this) > static_cast<double>(rhs);
  }

  bool isZero() const {
    return (this->getMant() == 0 && this->getExp() == 0);
  }
  bool isSubN() const {
    return (this->getMant() != 0 && this->getExp() == 0);
  }
  bool isInf() const {
    return (this->getMant() == 0
        && this->getExp() == (MASK_LOWER_HIGH(ExpType, this->prec.exp_size)));
  }
  bool isPinf() const {
    return (this->isInf() && this->getSign() == 0);
  }
  bool isNinf() const {
    return (this->isInf() && this->getSign() != 0);
  }
  bool isNaN() const {
    return (this->getMant() != 0
        && this->getExp() == (MASK_LOWER_HIGH(ExpType, this->prec.exp_size)));
  }

  void setZero() {
    this->setExp(0);
    this->setMant(0);
  }
  void setInf() {
    this->setMant(0);
    this->setExp((MASK_LOWER_HIGH(ExpType, this->prec.exp_size)));
  }
  void setPinf() {
    this->setInf();
    this->setSign(0);
  }
  void setNinf() {
    this->setInf();
    this->setSign(1);
  }
  void setNaN() {
    this->setInf();
    this->setMant(1);
  }

  void adaptPrec(VPA&);
  void changePrec(FloatPointPrecTy);

  static void test(float op1, float op2);
  static void test(double op1, double op2);

  void shift(int);
  void normalize(int, int);
  void round(VPA_rounding_method method = VPA_FP_ROUND_NEAREST);

};

}

::std::ostream& operator<<(::std::ostream&, const ::vpa::VPA&);


#endif //VPA_H