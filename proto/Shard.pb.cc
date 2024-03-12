// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Shard.proto

#include "Shard.pb.h"

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

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

PROTOBUF_CONSTEXPR Shard::Shard(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.tsm_file_)*/{}
  , /*decltype(_impl_.retention_policy_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.shard_id_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.database_name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.measurement_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.curr_file_path_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.curr_file_margin_)*/int64_t{0}
  , /*decltype(_impl_.curr_file_head_offset_)*/int64_t{0}
  , /*decltype(_impl_.curr_file_tail_offset_)*/int64_t{0}
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct ShardDefaultTypeInternal {
  PROTOBUF_CONSTEXPR ShardDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~ShardDefaultTypeInternal() {}
  union {
    Shard _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 ShardDefaultTypeInternal _Shard_default_instance_;
static ::_pb::Metadata file_level_metadata_Shard_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_Shard_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_Shard_2eproto = nullptr;

const uint32_t TableStruct_Shard_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Shard, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.retention_policy_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.shard_id_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.database_name_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.measurement_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.tsm_file_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.curr_file_path_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.curr_file_margin_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.curr_file_head_offset_),
  PROTOBUF_FIELD_OFFSET(::Shard, _impl_.curr_file_tail_offset_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::Shard)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::_Shard_default_instance_._instance,
};

const char descriptor_table_protodef_Shard_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\013Shard.proto\"\341\001\n\005Shard\022\030\n\020retention_pol"
  "icy\030\001 \001(\014\022\020\n\010shard_id\030\002 \001(\014\022\025\n\rdatabase_"
  "name\030\003 \001(\014\022\023\n\013measurement\030\004 \001(\014\022\020\n\010tsm_f"
  "ile\030\005 \003(\014\022\026\n\016curr_file_path\030\006 \001(\014\022\030\n\020cur"
  "r_file_margin\030\007 \001(\003\022\035\n\025curr_file_head_of"
  "fset\030\010 \001(\003\022\035\n\025curr_file_tail_offset\030\t \001("
  "\003b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_Shard_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_Shard_2eproto = {
    false, false, 249, descriptor_table_protodef_Shard_2eproto,
    "Shard.proto",
    &descriptor_table_Shard_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_Shard_2eproto::offsets,
    file_level_metadata_Shard_2eproto, file_level_enum_descriptors_Shard_2eproto,
    file_level_service_descriptors_Shard_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_Shard_2eproto_getter() {
  return &descriptor_table_Shard_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_Shard_2eproto(&descriptor_table_Shard_2eproto);

// ===================================================================

class Shard::_Internal {
 public:
};

Shard::Shard(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:Shard)
}
Shard::Shard(const Shard& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Shard* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.tsm_file_){from._impl_.tsm_file_}
    , decltype(_impl_.retention_policy_){}
    , decltype(_impl_.shard_id_){}
    , decltype(_impl_.database_name_){}
    , decltype(_impl_.measurement_){}
    , decltype(_impl_.curr_file_path_){}
    , decltype(_impl_.curr_file_margin_){}
    , decltype(_impl_.curr_file_head_offset_){}
    , decltype(_impl_.curr_file_tail_offset_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.retention_policy_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.retention_policy_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_retention_policy().empty()) {
    _this->_impl_.retention_policy_.Set(from._internal_retention_policy(), 
      _this->GetArenaForAllocation());
  }
  _impl_.shard_id_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.shard_id_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_shard_id().empty()) {
    _this->_impl_.shard_id_.Set(from._internal_shard_id(), 
      _this->GetArenaForAllocation());
  }
  _impl_.database_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.database_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_database_name().empty()) {
    _this->_impl_.database_name_.Set(from._internal_database_name(), 
      _this->GetArenaForAllocation());
  }
  _impl_.measurement_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.measurement_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_measurement().empty()) {
    _this->_impl_.measurement_.Set(from._internal_measurement(), 
      _this->GetArenaForAllocation());
  }
  _impl_.curr_file_path_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.curr_file_path_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_curr_file_path().empty()) {
    _this->_impl_.curr_file_path_.Set(from._internal_curr_file_path(), 
      _this->GetArenaForAllocation());
  }
  ::memcpy(&_impl_.curr_file_margin_, &from._impl_.curr_file_margin_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.curr_file_tail_offset_) -
    reinterpret_cast<char*>(&_impl_.curr_file_margin_)) + sizeof(_impl_.curr_file_tail_offset_));
  // @@protoc_insertion_point(copy_constructor:Shard)
}

inline void Shard::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.tsm_file_){arena}
    , decltype(_impl_.retention_policy_){}
    , decltype(_impl_.shard_id_){}
    , decltype(_impl_.database_name_){}
    , decltype(_impl_.measurement_){}
    , decltype(_impl_.curr_file_path_){}
    , decltype(_impl_.curr_file_margin_){int64_t{0}}
    , decltype(_impl_.curr_file_head_offset_){int64_t{0}}
    , decltype(_impl_.curr_file_tail_offset_){int64_t{0}}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.retention_policy_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.retention_policy_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.shard_id_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.shard_id_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.database_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.database_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.measurement_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.measurement_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.curr_file_path_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.curr_file_path_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

Shard::~Shard() {
  // @@protoc_insertion_point(destructor:Shard)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Shard::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.tsm_file_.~RepeatedPtrField();
  _impl_.retention_policy_.Destroy();
  _impl_.shard_id_.Destroy();
  _impl_.database_name_.Destroy();
  _impl_.measurement_.Destroy();
  _impl_.curr_file_path_.Destroy();
}

void Shard::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Shard::Clear() {
// @@protoc_insertion_point(message_clear_start:Shard)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.tsm_file_.Clear();
  _impl_.retention_policy_.ClearToEmpty();
  _impl_.shard_id_.ClearToEmpty();
  _impl_.database_name_.ClearToEmpty();
  _impl_.measurement_.ClearToEmpty();
  _impl_.curr_file_path_.ClearToEmpty();
  ::memset(&_impl_.curr_file_margin_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.curr_file_tail_offset_) -
      reinterpret_cast<char*>(&_impl_.curr_file_margin_)) + sizeof(_impl_.curr_file_tail_offset_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Shard::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // bytes retention_policy = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_retention_policy();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // bytes shard_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          auto str = _internal_mutable_shard_id();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // bytes database_name = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          auto str = _internal_mutable_database_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // bytes measurement = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          auto str = _internal_mutable_measurement();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // repeated bytes tsm_file = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 42)) {
          ptr -= 1;
          do {
            ptr += 1;
            auto str = _internal_add_tsm_file();
            ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<42>(ptr));
        } else
          goto handle_unusual;
        continue;
      // bytes curr_file_path = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 50)) {
          auto str = _internal_mutable_curr_file_path();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 curr_file_margin = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 56)) {
          _impl_.curr_file_margin_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 curr_file_head_offset = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 64)) {
          _impl_.curr_file_head_offset_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 curr_file_tail_offset = 9;
      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 72)) {
          _impl_.curr_file_tail_offset_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Shard::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Shard)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // bytes retention_policy = 1;
  if (!this->_internal_retention_policy().empty()) {
    target = stream->WriteBytesMaybeAliased(
        1, this->_internal_retention_policy(), target);
  }

  // bytes shard_id = 2;
  if (!this->_internal_shard_id().empty()) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_shard_id(), target);
  }

  // bytes database_name = 3;
  if (!this->_internal_database_name().empty()) {
    target = stream->WriteBytesMaybeAliased(
        3, this->_internal_database_name(), target);
  }

  // bytes measurement = 4;
  if (!this->_internal_measurement().empty()) {
    target = stream->WriteBytesMaybeAliased(
        4, this->_internal_measurement(), target);
  }

  // repeated bytes tsm_file = 5;
  for (int i = 0, n = this->_internal_tsm_file_size(); i < n; i++) {
    const auto& s = this->_internal_tsm_file(i);
    target = stream->WriteBytes(5, s, target);
  }

  // bytes curr_file_path = 6;
  if (!this->_internal_curr_file_path().empty()) {
    target = stream->WriteBytesMaybeAliased(
        6, this->_internal_curr_file_path(), target);
  }

  // int64 curr_file_margin = 7;
  if (this->_internal_curr_file_margin() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(7, this->_internal_curr_file_margin(), target);
  }

  // int64 curr_file_head_offset = 8;
  if (this->_internal_curr_file_head_offset() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(8, this->_internal_curr_file_head_offset(), target);
  }

  // int64 curr_file_tail_offset = 9;
  if (this->_internal_curr_file_tail_offset() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(9, this->_internal_curr_file_tail_offset(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Shard)
  return target;
}

size_t Shard::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Shard)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated bytes tsm_file = 5;
  total_size += 1 *
      ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(_impl_.tsm_file_.size());
  for (int i = 0, n = _impl_.tsm_file_.size(); i < n; i++) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
      _impl_.tsm_file_.Get(i));
  }

  // bytes retention_policy = 1;
  if (!this->_internal_retention_policy().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_retention_policy());
  }

  // bytes shard_id = 2;
  if (!this->_internal_shard_id().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_shard_id());
  }

  // bytes database_name = 3;
  if (!this->_internal_database_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_database_name());
  }

  // bytes measurement = 4;
  if (!this->_internal_measurement().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_measurement());
  }

  // bytes curr_file_path = 6;
  if (!this->_internal_curr_file_path().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_curr_file_path());
  }

  // int64 curr_file_margin = 7;
  if (this->_internal_curr_file_margin() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(this->_internal_curr_file_margin());
  }

  // int64 curr_file_head_offset = 8;
  if (this->_internal_curr_file_head_offset() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(this->_internal_curr_file_head_offset());
  }

  // int64 curr_file_tail_offset = 9;
  if (this->_internal_curr_file_tail_offset() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(this->_internal_curr_file_tail_offset());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Shard::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Shard::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Shard::GetClassData() const { return &_class_data_; }


void Shard::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Shard*>(&to_msg);
  auto& from = static_cast<const Shard&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Shard)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.tsm_file_.MergeFrom(from._impl_.tsm_file_);
  if (!from._internal_retention_policy().empty()) {
    _this->_internal_set_retention_policy(from._internal_retention_policy());
  }
  if (!from._internal_shard_id().empty()) {
    _this->_internal_set_shard_id(from._internal_shard_id());
  }
  if (!from._internal_database_name().empty()) {
    _this->_internal_set_database_name(from._internal_database_name());
  }
  if (!from._internal_measurement().empty()) {
    _this->_internal_set_measurement(from._internal_measurement());
  }
  if (!from._internal_curr_file_path().empty()) {
    _this->_internal_set_curr_file_path(from._internal_curr_file_path());
  }
  if (from._internal_curr_file_margin() != 0) {
    _this->_internal_set_curr_file_margin(from._internal_curr_file_margin());
  }
  if (from._internal_curr_file_head_offset() != 0) {
    _this->_internal_set_curr_file_head_offset(from._internal_curr_file_head_offset());
  }
  if (from._internal_curr_file_tail_offset() != 0) {
    _this->_internal_set_curr_file_tail_offset(from._internal_curr_file_tail_offset());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Shard::CopyFrom(const Shard& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Shard)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Shard::IsInitialized() const {
  return true;
}

void Shard::InternalSwap(Shard* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.tsm_file_.InternalSwap(&other->_impl_.tsm_file_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.retention_policy_, lhs_arena,
      &other->_impl_.retention_policy_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.shard_id_, lhs_arena,
      &other->_impl_.shard_id_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.database_name_, lhs_arena,
      &other->_impl_.database_name_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.measurement_, lhs_arena,
      &other->_impl_.measurement_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.curr_file_path_, lhs_arena,
      &other->_impl_.curr_file_path_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Shard, _impl_.curr_file_tail_offset_)
      + sizeof(Shard::_impl_.curr_file_tail_offset_)
      - PROTOBUF_FIELD_OFFSET(Shard, _impl_.curr_file_margin_)>(
          reinterpret_cast<char*>(&_impl_.curr_file_margin_),
          reinterpret_cast<char*>(&other->_impl_.curr_file_margin_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Shard::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Shard_2eproto_getter, &descriptor_table_Shard_2eproto_once,
      file_level_metadata_Shard_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Shard*
Arena::CreateMaybeMessage< ::Shard >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Shard >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>