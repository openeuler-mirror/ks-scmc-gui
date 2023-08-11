// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_common_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_common_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012003 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_common_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_common_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_common_2eproto;
namespace common {
class ReplyHeader;
class ReplyHeaderDefaultTypeInternal;
extern ReplyHeaderDefaultTypeInternal _ReplyHeader_default_instance_;
class RequestHeader;
class RequestHeaderDefaultTypeInternal;
extern RequestHeaderDefaultTypeInternal _RequestHeader_default_instance_;
}  // namespace common
PROTOBUF_NAMESPACE_OPEN
template<> ::common::ReplyHeader* Arena::CreateMaybeMessage<::common::ReplyHeader>(Arena*);
template<> ::common::RequestHeader* Arena::CreateMaybeMessage<::common::RequestHeader>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace common {

enum Errno : int {
  OK = 0,
  Canceled = 1,
  Unknown = 2,
  InvalidArgument = 3,
  DeadlineExceeded = 4,
  NotFound = 5,
  AlreadyExists = 6,
  PermissionDenied = 7,
  ResourceExhausted = 8,
  FailedPrecondition = 9,
  Aborted = 10,
  OutOfRange = 11,
  Unimplemented = 12,
  Internal = 13,
  Unavailable = 14,
  DataLoss = 15,
  Unauthenticated = 16,
  UserAlreadyExist = 201,
  UserNotExist = 202,
  WrongPassword = 203,
  InvalidSession = 204,
  SessionExpired = 205,
  NoPermission = 206,
  Errno_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  Errno_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool Errno_IsValid(int value);
constexpr Errno Errno_MIN = OK;
constexpr Errno Errno_MAX = NoPermission;
constexpr int Errno_ARRAYSIZE = Errno_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Errno_descriptor();
template<typename T>
inline const std::string& Errno_Name(T enum_t_value) {
  static_assert(::std::is_same<T, Errno>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function Errno_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    Errno_descriptor(), enum_t_value);
}
inline bool Errno_Parse(
    const std::string& name, Errno* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<Errno>(
    Errno_descriptor(), name, value);
}
// ===================================================================

class RequestHeader PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:common.RequestHeader) */ {
 public:
  inline RequestHeader() : RequestHeader(nullptr) {};
  virtual ~RequestHeader();

  RequestHeader(const RequestHeader& from);
  RequestHeader(RequestHeader&& from) noexcept
    : RequestHeader() {
    *this = ::std::move(from);
  }

  inline RequestHeader& operator=(const RequestHeader& from) {
    CopyFrom(from);
    return *this;
  }
  inline RequestHeader& operator=(RequestHeader&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const RequestHeader& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const RequestHeader* internal_default_instance() {
    return reinterpret_cast<const RequestHeader*>(
               &_RequestHeader_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(RequestHeader& a, RequestHeader& b) {
    a.Swap(&b);
  }
  inline void Swap(RequestHeader* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(RequestHeader* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline RequestHeader* New() const final {
    return CreateMaybeMessage<RequestHeader>(nullptr);
  }

  RequestHeader* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<RequestHeader>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const RequestHeader& from);
  void MergeFrom(const RequestHeader& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(RequestHeader* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "common.RequestHeader";
  }
  protected:
  explicit RequestHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_common_2eproto);
    return ::descriptor_table_common_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kClientTypeFieldNumber = 1,
    kClientVersionFieldNumber = 2,
    kClientKeyFieldNumber = 3,
    kSessionKeyFieldNumber = 11,
    kUserIdFieldNumber = 10,
  };
  // string client_type = 1;
  void clear_client_type();
  const std::string& client_type() const;
  void set_client_type(const std::string& value);
  void set_client_type(std::string&& value);
  void set_client_type(const char* value);
  void set_client_type(const char* value, size_t size);
  std::string* mutable_client_type();
  std::string* release_client_type();
  void set_allocated_client_type(std::string* client_type);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_client_type();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_client_type(
      std::string* client_type);
  private:
  const std::string& _internal_client_type() const;
  void _internal_set_client_type(const std::string& value);
  std::string* _internal_mutable_client_type();
  public:

  // string client_version = 2;
  void clear_client_version();
  const std::string& client_version() const;
  void set_client_version(const std::string& value);
  void set_client_version(std::string&& value);
  void set_client_version(const char* value);
  void set_client_version(const char* value, size_t size);
  std::string* mutable_client_version();
  std::string* release_client_version();
  void set_allocated_client_version(std::string* client_version);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_client_version();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_client_version(
      std::string* client_version);
  private:
  const std::string& _internal_client_version() const;
  void _internal_set_client_version(const std::string& value);
  std::string* _internal_mutable_client_version();
  public:

  // string client_key = 3;
  void clear_client_key();
  const std::string& client_key() const;
  void set_client_key(const std::string& value);
  void set_client_key(std::string&& value);
  void set_client_key(const char* value);
  void set_client_key(const char* value, size_t size);
  std::string* mutable_client_key();
  std::string* release_client_key();
  void set_allocated_client_key(std::string* client_key);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_client_key();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_client_key(
      std::string* client_key);
  private:
  const std::string& _internal_client_key() const;
  void _internal_set_client_key(const std::string& value);
  std::string* _internal_mutable_client_key();
  public:

  // string session_key = 11;
  void clear_session_key();
  const std::string& session_key() const;
  void set_session_key(const std::string& value);
  void set_session_key(std::string&& value);
  void set_session_key(const char* value);
  void set_session_key(const char* value, size_t size);
  std::string* mutable_session_key();
  std::string* release_session_key();
  void set_allocated_session_key(std::string* session_key);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_session_key();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_session_key(
      std::string* session_key);
  private:
  const std::string& _internal_session_key() const;
  void _internal_set_session_key(const std::string& value);
  std::string* _internal_mutable_session_key();
  public:

  // int64 user_id = 10;
  void clear_user_id();
  ::PROTOBUF_NAMESPACE_ID::int64 user_id() const;
  void set_user_id(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_user_id() const;
  void _internal_set_user_id(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // @@protoc_insertion_point(class_scope:common.RequestHeader)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr client_type_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr client_version_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr client_key_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr session_key_;
  ::PROTOBUF_NAMESPACE_ID::int64 user_id_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_common_2eproto;
};
// -------------------------------------------------------------------

class ReplyHeader PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:common.ReplyHeader) */ {
 public:
  inline ReplyHeader() : ReplyHeader(nullptr) {};
  virtual ~ReplyHeader();

  ReplyHeader(const ReplyHeader& from);
  ReplyHeader(ReplyHeader&& from) noexcept
    : ReplyHeader() {
    *this = ::std::move(from);
  }

  inline ReplyHeader& operator=(const ReplyHeader& from) {
    CopyFrom(from);
    return *this;
  }
  inline ReplyHeader& operator=(ReplyHeader&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const ReplyHeader& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ReplyHeader* internal_default_instance() {
    return reinterpret_cast<const ReplyHeader*>(
               &_ReplyHeader_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(ReplyHeader& a, ReplyHeader& b) {
    a.Swap(&b);
  }
  inline void Swap(ReplyHeader* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ReplyHeader* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline ReplyHeader* New() const final {
    return CreateMaybeMessage<ReplyHeader>(nullptr);
  }

  ReplyHeader* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<ReplyHeader>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const ReplyHeader& from);
  void MergeFrom(const ReplyHeader& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ReplyHeader* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "common.ReplyHeader";
  }
  protected:
  explicit ReplyHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_common_2eproto);
    return ::descriptor_table_common_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kErrMsgFieldNumber = 2,
    kErrNoFieldNumber = 1,
  };
  // string err_msg = 2;
  void clear_err_msg();
  const std::string& err_msg() const;
  void set_err_msg(const std::string& value);
  void set_err_msg(std::string&& value);
  void set_err_msg(const char* value);
  void set_err_msg(const char* value, size_t size);
  std::string* mutable_err_msg();
  std::string* release_err_msg();
  void set_allocated_err_msg(std::string* err_msg);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_err_msg();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_err_msg(
      std::string* err_msg);
  private:
  const std::string& _internal_err_msg() const;
  void _internal_set_err_msg(const std::string& value);
  std::string* _internal_mutable_err_msg();
  public:

  // int32 err_no = 1;
  void clear_err_no();
  ::PROTOBUF_NAMESPACE_ID::int32 err_no() const;
  void set_err_no(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_err_no() const;
  void _internal_set_err_no(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:common.ReplyHeader)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr err_msg_;
  ::PROTOBUF_NAMESPACE_ID::int32 err_no_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_common_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// RequestHeader

// string client_type = 1;
inline void RequestHeader::clear_client_type() {
  client_type_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& RequestHeader::client_type() const {
  // @@protoc_insertion_point(field_get:common.RequestHeader.client_type)
  return _internal_client_type();
}
inline void RequestHeader::set_client_type(const std::string& value) {
  _internal_set_client_type(value);
  // @@protoc_insertion_point(field_set:common.RequestHeader.client_type)
}
inline std::string* RequestHeader::mutable_client_type() {
  // @@protoc_insertion_point(field_mutable:common.RequestHeader.client_type)
  return _internal_mutable_client_type();
}
inline const std::string& RequestHeader::_internal_client_type() const {
  return client_type_.Get();
}
inline void RequestHeader::_internal_set_client_type(const std::string& value) {
  
  client_type_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void RequestHeader::set_client_type(std::string&& value) {
  
  client_type_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:common.RequestHeader.client_type)
}
inline void RequestHeader::set_client_type(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  client_type_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:common.RequestHeader.client_type)
}
inline void RequestHeader::set_client_type(const char* value,
    size_t size) {
  
  client_type_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:common.RequestHeader.client_type)
}
inline std::string* RequestHeader::_internal_mutable_client_type() {
  
  return client_type_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* RequestHeader::release_client_type() {
  // @@protoc_insertion_point(field_release:common.RequestHeader.client_type)
  return client_type_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RequestHeader::set_allocated_client_type(std::string* client_type) {
  if (client_type != nullptr) {
    
  } else {
    
  }
  client_type_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), client_type,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:common.RequestHeader.client_type)
}
inline std::string* RequestHeader::unsafe_arena_release_client_type() {
  // @@protoc_insertion_point(field_unsafe_arena_release:common.RequestHeader.client_type)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return client_type_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void RequestHeader::unsafe_arena_set_allocated_client_type(
    std::string* client_type) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (client_type != nullptr) {
    
  } else {
    
  }
  client_type_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      client_type, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:common.RequestHeader.client_type)
}

// string client_version = 2;
inline void RequestHeader::clear_client_version() {
  client_version_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& RequestHeader::client_version() const {
  // @@protoc_insertion_point(field_get:common.RequestHeader.client_version)
  return _internal_client_version();
}
inline void RequestHeader::set_client_version(const std::string& value) {
  _internal_set_client_version(value);
  // @@protoc_insertion_point(field_set:common.RequestHeader.client_version)
}
inline std::string* RequestHeader::mutable_client_version() {
  // @@protoc_insertion_point(field_mutable:common.RequestHeader.client_version)
  return _internal_mutable_client_version();
}
inline const std::string& RequestHeader::_internal_client_version() const {
  return client_version_.Get();
}
inline void RequestHeader::_internal_set_client_version(const std::string& value) {
  
  client_version_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void RequestHeader::set_client_version(std::string&& value) {
  
  client_version_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:common.RequestHeader.client_version)
}
inline void RequestHeader::set_client_version(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  client_version_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:common.RequestHeader.client_version)
}
inline void RequestHeader::set_client_version(const char* value,
    size_t size) {
  
  client_version_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:common.RequestHeader.client_version)
}
inline std::string* RequestHeader::_internal_mutable_client_version() {
  
  return client_version_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* RequestHeader::release_client_version() {
  // @@protoc_insertion_point(field_release:common.RequestHeader.client_version)
  return client_version_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RequestHeader::set_allocated_client_version(std::string* client_version) {
  if (client_version != nullptr) {
    
  } else {
    
  }
  client_version_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), client_version,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:common.RequestHeader.client_version)
}
inline std::string* RequestHeader::unsafe_arena_release_client_version() {
  // @@protoc_insertion_point(field_unsafe_arena_release:common.RequestHeader.client_version)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return client_version_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void RequestHeader::unsafe_arena_set_allocated_client_version(
    std::string* client_version) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (client_version != nullptr) {
    
  } else {
    
  }
  client_version_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      client_version, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:common.RequestHeader.client_version)
}

// string client_key = 3;
inline void RequestHeader::clear_client_key() {
  client_key_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& RequestHeader::client_key() const {
  // @@protoc_insertion_point(field_get:common.RequestHeader.client_key)
  return _internal_client_key();
}
inline void RequestHeader::set_client_key(const std::string& value) {
  _internal_set_client_key(value);
  // @@protoc_insertion_point(field_set:common.RequestHeader.client_key)
}
inline std::string* RequestHeader::mutable_client_key() {
  // @@protoc_insertion_point(field_mutable:common.RequestHeader.client_key)
  return _internal_mutable_client_key();
}
inline const std::string& RequestHeader::_internal_client_key() const {
  return client_key_.Get();
}
inline void RequestHeader::_internal_set_client_key(const std::string& value) {
  
  client_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void RequestHeader::set_client_key(std::string&& value) {
  
  client_key_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:common.RequestHeader.client_key)
}
inline void RequestHeader::set_client_key(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  client_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:common.RequestHeader.client_key)
}
inline void RequestHeader::set_client_key(const char* value,
    size_t size) {
  
  client_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:common.RequestHeader.client_key)
}
inline std::string* RequestHeader::_internal_mutable_client_key() {
  
  return client_key_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* RequestHeader::release_client_key() {
  // @@protoc_insertion_point(field_release:common.RequestHeader.client_key)
  return client_key_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RequestHeader::set_allocated_client_key(std::string* client_key) {
  if (client_key != nullptr) {
    
  } else {
    
  }
  client_key_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), client_key,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:common.RequestHeader.client_key)
}
inline std::string* RequestHeader::unsafe_arena_release_client_key() {
  // @@protoc_insertion_point(field_unsafe_arena_release:common.RequestHeader.client_key)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return client_key_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void RequestHeader::unsafe_arena_set_allocated_client_key(
    std::string* client_key) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (client_key != nullptr) {
    
  } else {
    
  }
  client_key_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      client_key, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:common.RequestHeader.client_key)
}

// int64 user_id = 10;
inline void RequestHeader::clear_user_id() {
  user_id_ = PROTOBUF_LONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 RequestHeader::_internal_user_id() const {
  return user_id_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 RequestHeader::user_id() const {
  // @@protoc_insertion_point(field_get:common.RequestHeader.user_id)
  return _internal_user_id();
}
inline void RequestHeader::_internal_set_user_id(::PROTOBUF_NAMESPACE_ID::int64 value) {
  
  user_id_ = value;
}
inline void RequestHeader::set_user_id(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_user_id(value);
  // @@protoc_insertion_point(field_set:common.RequestHeader.user_id)
}

// string session_key = 11;
inline void RequestHeader::clear_session_key() {
  session_key_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& RequestHeader::session_key() const {
  // @@protoc_insertion_point(field_get:common.RequestHeader.session_key)
  return _internal_session_key();
}
inline void RequestHeader::set_session_key(const std::string& value) {
  _internal_set_session_key(value);
  // @@protoc_insertion_point(field_set:common.RequestHeader.session_key)
}
inline std::string* RequestHeader::mutable_session_key() {
  // @@protoc_insertion_point(field_mutable:common.RequestHeader.session_key)
  return _internal_mutable_session_key();
}
inline const std::string& RequestHeader::_internal_session_key() const {
  return session_key_.Get();
}
inline void RequestHeader::_internal_set_session_key(const std::string& value) {
  
  session_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void RequestHeader::set_session_key(std::string&& value) {
  
  session_key_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:common.RequestHeader.session_key)
}
inline void RequestHeader::set_session_key(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  session_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:common.RequestHeader.session_key)
}
inline void RequestHeader::set_session_key(const char* value,
    size_t size) {
  
  session_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:common.RequestHeader.session_key)
}
inline std::string* RequestHeader::_internal_mutable_session_key() {
  
  return session_key_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* RequestHeader::release_session_key() {
  // @@protoc_insertion_point(field_release:common.RequestHeader.session_key)
  return session_key_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RequestHeader::set_allocated_session_key(std::string* session_key) {
  if (session_key != nullptr) {
    
  } else {
    
  }
  session_key_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), session_key,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:common.RequestHeader.session_key)
}
inline std::string* RequestHeader::unsafe_arena_release_session_key() {
  // @@protoc_insertion_point(field_unsafe_arena_release:common.RequestHeader.session_key)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return session_key_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void RequestHeader::unsafe_arena_set_allocated_session_key(
    std::string* session_key) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (session_key != nullptr) {
    
  } else {
    
  }
  session_key_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      session_key, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:common.RequestHeader.session_key)
}

// -------------------------------------------------------------------

// ReplyHeader

// int32 err_no = 1;
inline void ReplyHeader::clear_err_no() {
  err_no_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 ReplyHeader::_internal_err_no() const {
  return err_no_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 ReplyHeader::err_no() const {
  // @@protoc_insertion_point(field_get:common.ReplyHeader.err_no)
  return _internal_err_no();
}
inline void ReplyHeader::_internal_set_err_no(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  err_no_ = value;
}
inline void ReplyHeader::set_err_no(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_err_no(value);
  // @@protoc_insertion_point(field_set:common.ReplyHeader.err_no)
}

// string err_msg = 2;
inline void ReplyHeader::clear_err_msg() {
  err_msg_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& ReplyHeader::err_msg() const {
  // @@protoc_insertion_point(field_get:common.ReplyHeader.err_msg)
  return _internal_err_msg();
}
inline void ReplyHeader::set_err_msg(const std::string& value) {
  _internal_set_err_msg(value);
  // @@protoc_insertion_point(field_set:common.ReplyHeader.err_msg)
}
inline std::string* ReplyHeader::mutable_err_msg() {
  // @@protoc_insertion_point(field_mutable:common.ReplyHeader.err_msg)
  return _internal_mutable_err_msg();
}
inline const std::string& ReplyHeader::_internal_err_msg() const {
  return err_msg_.Get();
}
inline void ReplyHeader::_internal_set_err_msg(const std::string& value) {
  
  err_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void ReplyHeader::set_err_msg(std::string&& value) {
  
  err_msg_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:common.ReplyHeader.err_msg)
}
inline void ReplyHeader::set_err_msg(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  err_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:common.ReplyHeader.err_msg)
}
inline void ReplyHeader::set_err_msg(const char* value,
    size_t size) {
  
  err_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:common.ReplyHeader.err_msg)
}
inline std::string* ReplyHeader::_internal_mutable_err_msg() {
  
  return err_msg_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* ReplyHeader::release_err_msg() {
  // @@protoc_insertion_point(field_release:common.ReplyHeader.err_msg)
  return err_msg_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void ReplyHeader::set_allocated_err_msg(std::string* err_msg) {
  if (err_msg != nullptr) {
    
  } else {
    
  }
  err_msg_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), err_msg,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:common.ReplyHeader.err_msg)
}
inline std::string* ReplyHeader::unsafe_arena_release_err_msg() {
  // @@protoc_insertion_point(field_unsafe_arena_release:common.ReplyHeader.err_msg)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return err_msg_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void ReplyHeader::unsafe_arena_set_allocated_err_msg(
    std::string* err_msg) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (err_msg != nullptr) {
    
  } else {
    
  }
  err_msg_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      err_msg, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:common.ReplyHeader.err_msg)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace common

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::common::Errno> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::common::Errno>() {
  return ::common::Errno_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_common_2eproto
