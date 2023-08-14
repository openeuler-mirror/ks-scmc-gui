// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common.proto

#include "common.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace common {
class RequestHeaderDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<RequestHeader> _instance;
} _RequestHeader_default_instance_;
class ReplyHeaderDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ReplyHeader> _instance;
} _ReplyHeader_default_instance_;
}  // namespace common
static void InitDefaultsscc_info_ReplyHeader_common_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::common::_ReplyHeader_default_instance_;
    new (ptr) ::common::ReplyHeader();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::common::ReplyHeader::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_ReplyHeader_common_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_ReplyHeader_common_2eproto}, {}};

static void InitDefaultsscc_info_RequestHeader_common_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::common::_RequestHeader_default_instance_;
    new (ptr) ::common::RequestHeader();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::common::RequestHeader::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_RequestHeader_common_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_RequestHeader_common_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_common_2eproto[2];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_common_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_common_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_common_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, client_type_),
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, client_version_),
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, client_key_),
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, user_id_),
  PROTOBUF_FIELD_OFFSET(::common::RequestHeader, session_key_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::common::ReplyHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::common::ReplyHeader, err_no_),
  PROTOBUF_FIELD_OFFSET(::common::ReplyHeader, err_msg_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::common::RequestHeader)},
  { 10, -1, sizeof(::common::ReplyHeader)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::common::_RequestHeader_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::common::_ReplyHeader_default_instance_),
};

const char descriptor_table_protodef_common_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014common.proto\022\006common\"v\n\rRequestHeader\022"
  "\023\n\013client_type\030\001 \001(\t\022\026\n\016client_version\030\002"
  " \001(\t\022\022\n\nclient_key\030\003 \001(\t\022\017\n\007user_id\030\n \001("
  "\003\022\023\n\013session_key\030\013 \001(\t\".\n\013ReplyHeader\022\016\n"
  "\006err_no\030\001 \001(\005\022\017\n\007err_msg\030\002 \001(\t*\250\003\n\005Errno"
  "\022\006\n\002OK\020\000\022\014\n\010Canceled\020\001\022\013\n\007Unknown\020\002\022\023\n\017I"
  "nvalidArgument\020\003\022\024\n\020DeadlineExceeded\020\004\022\014"
  "\n\010NotFound\020\005\022\021\n\rAlreadyExists\020\006\022\024\n\020Permi"
  "ssionDenied\020\007\022\025\n\021ResourceExhausted\020\010\022\026\n\022"
  "FailedPrecondition\020\t\022\013\n\007Aborted\020\n\022\016\n\nOut"
  "OfRange\020\013\022\021\n\rUnimplemented\020\014\022\014\n\010Internal"
  "\020\r\022\017\n\013Unavailable\020\016\022\014\n\010DataLoss\020\017\022\023\n\017Una"
  "uthenticated\020\020\022\025\n\020UserAlreadyExist\020\311\001\022\021\n"
  "\014UserNotExist\020\312\001\022\022\n\rWrongPassword\020\313\001\022\023\n\016"
  "InvalidSession\020\314\001\022\023\n\016SessionExpired\020\315\001\022\021"
  "\n\014NoPermission\020\316\001B\024Z\022scmc/rpc/pb/commonb"
  "\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_common_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_common_2eproto_sccs[2] = {
  &scc_info_ReplyHeader_common_2eproto.base,
  &scc_info_RequestHeader_common_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_common_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_common_2eproto = {
  false, false, descriptor_table_protodef_common_2eproto, "common.proto", 647,
  &descriptor_table_common_2eproto_once, descriptor_table_common_2eproto_sccs, descriptor_table_common_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_common_2eproto::offsets,
  file_level_metadata_common_2eproto, 2, file_level_enum_descriptors_common_2eproto, file_level_service_descriptors_common_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_common_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_common_2eproto)), true);
namespace common {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Errno_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_common_2eproto);
  return file_level_enum_descriptors_common_2eproto[0];
}
bool Errno_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void RequestHeader::InitAsDefaultInstance() {
}
class RequestHeader::_Internal {
 public:
};

RequestHeader::RequestHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:common.RequestHeader)
}
RequestHeader::RequestHeader(const RequestHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  client_type_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_client_type().empty()) {
    client_type_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_client_type(),
      GetArena());
  }
  client_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_client_version().empty()) {
    client_version_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_client_version(),
      GetArena());
  }
  client_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_client_key().empty()) {
    client_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_client_key(),
      GetArena());
  }
  session_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_session_key().empty()) {
    session_key_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_session_key(),
      GetArena());
  }
  user_id_ = from.user_id_;
  // @@protoc_insertion_point(copy_constructor:common.RequestHeader)
}

void RequestHeader::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_RequestHeader_common_2eproto.base);
  client_type_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  client_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  client_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  session_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  user_id_ = PROTOBUF_LONGLONG(0);
}

RequestHeader::~RequestHeader() {
  // @@protoc_insertion_point(destructor:common.RequestHeader)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void RequestHeader::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  client_type_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  client_version_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  client_key_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  session_key_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void RequestHeader::ArenaDtor(void* object) {
  RequestHeader* _this = reinterpret_cast< RequestHeader* >(object);
  (void)_this;
}
void RequestHeader::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void RequestHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const RequestHeader& RequestHeader::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_RequestHeader_common_2eproto.base);
  return *internal_default_instance();
}


void RequestHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:common.RequestHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  client_type_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  client_version_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  client_key_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  session_key_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  user_id_ = PROTOBUF_LONGLONG(0);
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RequestHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string client_type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_client_type();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "common.RequestHeader.client_type"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string client_version = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_client_version();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "common.RequestHeader.client_version"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string client_key = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_client_key();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "common.RequestHeader.client_key"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int64 user_id = 10;
      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          user_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string session_key = 11;
      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          auto str = _internal_mutable_session_key();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "common.RequestHeader.session_key"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RequestHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:common.RequestHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string client_type = 1;
  if (this->client_type().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_client_type().data(), static_cast<int>(this->_internal_client_type().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "common.RequestHeader.client_type");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_client_type(), target);
  }

  // string client_version = 2;
  if (this->client_version().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_client_version().data(), static_cast<int>(this->_internal_client_version().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "common.RequestHeader.client_version");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_client_version(), target);
  }

  // string client_key = 3;
  if (this->client_key().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_client_key().data(), static_cast<int>(this->_internal_client_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "common.RequestHeader.client_key");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_client_key(), target);
  }

  // int64 user_id = 10;
  if (this->user_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(10, this->_internal_user_id(), target);
  }

  // string session_key = 11;
  if (this->session_key().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_session_key().data(), static_cast<int>(this->_internal_session_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "common.RequestHeader.session_key");
    target = stream->WriteStringMaybeAliased(
        11, this->_internal_session_key(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:common.RequestHeader)
  return target;
}

size_t RequestHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:common.RequestHeader)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string client_type = 1;
  if (this->client_type().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_client_type());
  }

  // string client_version = 2;
  if (this->client_version().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_client_version());
  }

  // string client_key = 3;
  if (this->client_key().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_client_key());
  }

  // string session_key = 11;
  if (this->session_key().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_session_key());
  }

  // int64 user_id = 10;
  if (this->user_id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
        this->_internal_user_id());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RequestHeader::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:common.RequestHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const RequestHeader* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RequestHeader>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:common.RequestHeader)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:common.RequestHeader)
    MergeFrom(*source);
  }
}

void RequestHeader::MergeFrom(const RequestHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:common.RequestHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.client_type().size() > 0) {
    _internal_set_client_type(from._internal_client_type());
  }
  if (from.client_version().size() > 0) {
    _internal_set_client_version(from._internal_client_version());
  }
  if (from.client_key().size() > 0) {
    _internal_set_client_key(from._internal_client_key());
  }
  if (from.session_key().size() > 0) {
    _internal_set_session_key(from._internal_session_key());
  }
  if (from.user_id() != 0) {
    _internal_set_user_id(from._internal_user_id());
  }
}

void RequestHeader::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:common.RequestHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RequestHeader::CopyFrom(const RequestHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:common.RequestHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RequestHeader::IsInitialized() const {
  return true;
}

void RequestHeader::InternalSwap(RequestHeader* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  client_type_.Swap(&other->client_type_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  client_version_.Swap(&other->client_version_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  client_key_.Swap(&other->client_key_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  session_key_.Swap(&other->session_key_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(user_id_, other->user_id_);
}

::PROTOBUF_NAMESPACE_ID::Metadata RequestHeader::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void ReplyHeader::InitAsDefaultInstance() {
}
class ReplyHeader::_Internal {
 public:
};

ReplyHeader::ReplyHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:common.ReplyHeader)
}
ReplyHeader::ReplyHeader(const ReplyHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  err_msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_err_msg().empty()) {
    err_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_err_msg(),
      GetArena());
  }
  err_no_ = from.err_no_;
  // @@protoc_insertion_point(copy_constructor:common.ReplyHeader)
}

void ReplyHeader::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_ReplyHeader_common_2eproto.base);
  err_msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  err_no_ = 0;
}

ReplyHeader::~ReplyHeader() {
  // @@protoc_insertion_point(destructor:common.ReplyHeader)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void ReplyHeader::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  err_msg_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void ReplyHeader::ArenaDtor(void* object) {
  ReplyHeader* _this = reinterpret_cast< ReplyHeader* >(object);
  (void)_this;
}
void ReplyHeader::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void ReplyHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ReplyHeader& ReplyHeader::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ReplyHeader_common_2eproto.base);
  return *internal_default_instance();
}


void ReplyHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:common.ReplyHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  err_msg_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  err_no_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* ReplyHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 err_no = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          err_no_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string err_msg = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_err_msg();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "common.ReplyHeader.err_msg"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ReplyHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:common.ReplyHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 err_no = 1;
  if (this->err_no() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_err_no(), target);
  }

  // string err_msg = 2;
  if (this->err_msg().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_err_msg().data(), static_cast<int>(this->_internal_err_msg().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "common.ReplyHeader.err_msg");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_err_msg(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:common.ReplyHeader)
  return target;
}

size_t ReplyHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:common.ReplyHeader)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string err_msg = 2;
  if (this->err_msg().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_err_msg());
  }

  // int32 err_no = 1;
  if (this->err_no() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_err_no());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ReplyHeader::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:common.ReplyHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const ReplyHeader* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ReplyHeader>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:common.ReplyHeader)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:common.ReplyHeader)
    MergeFrom(*source);
  }
}

void ReplyHeader::MergeFrom(const ReplyHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:common.ReplyHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.err_msg().size() > 0) {
    _internal_set_err_msg(from._internal_err_msg());
  }
  if (from.err_no() != 0) {
    _internal_set_err_no(from._internal_err_no());
  }
}

void ReplyHeader::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:common.ReplyHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ReplyHeader::CopyFrom(const ReplyHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:common.ReplyHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ReplyHeader::IsInitialized() const {
  return true;
}

void ReplyHeader::InternalSwap(ReplyHeader* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  err_msg_.Swap(&other->err_msg_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(err_no_, other->err_no_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ReplyHeader::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace common
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::common::RequestHeader* Arena::CreateMaybeMessage< ::common::RequestHeader >(Arena* arena) {
  return Arena::CreateMessageInternal< ::common::RequestHeader >(arena);
}
template<> PROTOBUF_NOINLINE ::common::ReplyHeader* Arena::CreateMaybeMessage< ::common::ReplyHeader >(Arena* arena) {
  return Arena::CreateMessageInternal< ::common::ReplyHeader >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
