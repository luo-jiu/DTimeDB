// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Person.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Person_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Person_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Person_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Person_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Person_2eproto;
class Person;
struct PersonDefaultTypeInternal;
extern PersonDefaultTypeInternal _Person_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Person* Arena::CreateMaybeMessage<::Person>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Person final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Person) */ {
 public:
  inline Person() : Person(nullptr) {}
  ~Person() override;
  explicit PROTOBUF_CONSTEXPR Person(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Person(const Person& from);
  Person(Person&& from) noexcept
    : Person() {
    *this = ::std::move(from);
  }

  inline Person& operator=(const Person& from) {
    CopyFrom(from);
    return *this;
  }
  inline Person& operator=(Person&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Person& default_instance() {
    return *internal_default_instance();
  }
  static inline const Person* internal_default_instance() {
    return reinterpret_cast<const Person*>(
               &_Person_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Person& a, Person& b) {
    a.Swap(&b);
  }
  inline void Swap(Person* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Person* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Person* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Person>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Person& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Person& from) {
    Person::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Person* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Person";
  }
  protected:
  explicit Person(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTsmFileFieldNumber = 5,
    kRetentionPolicyFieldNumber = 1,
    kShardIdFieldNumber = 2,
    kDatabaseNameFieldNumber = 3,
    kMeasurementFieldNumber = 4,
    kCurrFilePathFieldNumber = 6,
    kCurrFileMarginFieldNumber = 7,
    kCurrFileHeadOffsetFieldNumber = 8,
    kCurrFileTailOffsetFieldNumber = 9,
  };
  // repeated bytes tsm_file = 5;
  int tsm_file_size() const;
  private:
  int _internal_tsm_file_size() const;
  public:
  void clear_tsm_file();
  const std::string& tsm_file(int index) const;
  std::string* mutable_tsm_file(int index);
  void set_tsm_file(int index, const std::string& value);
  void set_tsm_file(int index, std::string&& value);
  void set_tsm_file(int index, const char* value);
  void set_tsm_file(int index, const void* value, size_t size);
  std::string* add_tsm_file();
  void add_tsm_file(const std::string& value);
  void add_tsm_file(std::string&& value);
  void add_tsm_file(const char* value);
  void add_tsm_file(const void* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& tsm_file() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_tsm_file();
  private:
  const std::string& _internal_tsm_file(int index) const;
  std::string* _internal_add_tsm_file();
  public:

  // bytes retention_policy = 1;
  void clear_retention_policy();
  const std::string& retention_policy() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_retention_policy(ArgT0&& arg0, ArgT... args);
  std::string* mutable_retention_policy();
  PROTOBUF_NODISCARD std::string* release_retention_policy();
  void set_allocated_retention_policy(std::string* retention_policy);
  private:
  const std::string& _internal_retention_policy() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_retention_policy(const std::string& value);
  std::string* _internal_mutable_retention_policy();
  public:

  // bytes shard_id = 2;
  void clear_shard_id();
  const std::string& shard_id() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_shard_id(ArgT0&& arg0, ArgT... args);
  std::string* mutable_shard_id();
  PROTOBUF_NODISCARD std::string* release_shard_id();
  void set_allocated_shard_id(std::string* shard_id);
  private:
  const std::string& _internal_shard_id() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_shard_id(const std::string& value);
  std::string* _internal_mutable_shard_id();
  public:

  // bytes database_name = 3;
  void clear_database_name();
  const std::string& database_name() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_database_name(ArgT0&& arg0, ArgT... args);
  std::string* mutable_database_name();
  PROTOBUF_NODISCARD std::string* release_database_name();
  void set_allocated_database_name(std::string* database_name);
  private:
  const std::string& _internal_database_name() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_database_name(const std::string& value);
  std::string* _internal_mutable_database_name();
  public:

  // bytes measurement = 4;
  void clear_measurement();
  const std::string& measurement() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_measurement(ArgT0&& arg0, ArgT... args);
  std::string* mutable_measurement();
  PROTOBUF_NODISCARD std::string* release_measurement();
  void set_allocated_measurement(std::string* measurement);
  private:
  const std::string& _internal_measurement() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_measurement(const std::string& value);
  std::string* _internal_mutable_measurement();
  public:

  // bytes curr_file_path = 6;
  void clear_curr_file_path();
  const std::string& curr_file_path() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_curr_file_path(ArgT0&& arg0, ArgT... args);
  std::string* mutable_curr_file_path();
  PROTOBUF_NODISCARD std::string* release_curr_file_path();
  void set_allocated_curr_file_path(std::string* curr_file_path);
  private:
  const std::string& _internal_curr_file_path() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_curr_file_path(const std::string& value);
  std::string* _internal_mutable_curr_file_path();
  public:

  // int64 curr_file_margin = 7;
  void clear_curr_file_margin();
  int64_t curr_file_margin() const;
  void set_curr_file_margin(int64_t value);
  private:
  int64_t _internal_curr_file_margin() const;
  void _internal_set_curr_file_margin(int64_t value);
  public:

  // int64 curr_file_head_offset = 8;
  void clear_curr_file_head_offset();
  int64_t curr_file_head_offset() const;
  void set_curr_file_head_offset(int64_t value);
  private:
  int64_t _internal_curr_file_head_offset() const;
  void _internal_set_curr_file_head_offset(int64_t value);
  public:

  // int64 curr_file_tail_offset = 9;
  void clear_curr_file_tail_offset();
  int64_t curr_file_tail_offset() const;
  void set_curr_file_tail_offset(int64_t value);
  private:
  int64_t _internal_curr_file_tail_offset() const;
  void _internal_set_curr_file_tail_offset(int64_t value);
  public:

  // @@protoc_insertion_point(class_scope:Person)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> tsm_file_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr retention_policy_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr shard_id_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr database_name_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr measurement_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr curr_file_path_;
    int64_t curr_file_margin_;
    int64_t curr_file_head_offset_;
    int64_t curr_file_tail_offset_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_Person_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Person

// bytes retention_policy = 1;
inline void Person::clear_retention_policy() {
  _impl_.retention_policy_.ClearToEmpty();
}
inline const std::string& Person::retention_policy() const {
  // @@protoc_insertion_point(field_get:Person.retention_policy)
  return _internal_retention_policy();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_retention_policy(ArgT0&& arg0, ArgT... args) {
 
 _impl_.retention_policy_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Person.retention_policy)
}
inline std::string* Person::mutable_retention_policy() {
  std::string* _s = _internal_mutable_retention_policy();
  // @@protoc_insertion_point(field_mutable:Person.retention_policy)
  return _s;
}
inline const std::string& Person::_internal_retention_policy() const {
  return _impl_.retention_policy_.Get();
}
inline void Person::_internal_set_retention_policy(const std::string& value) {
  
  _impl_.retention_policy_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_retention_policy() {
  
  return _impl_.retention_policy_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_retention_policy() {
  // @@protoc_insertion_point(field_release:Person.retention_policy)
  return _impl_.retention_policy_.Release();
}
inline void Person::set_allocated_retention_policy(std::string* retention_policy) {
  if (retention_policy != nullptr) {
    
  } else {
    
  }
  _impl_.retention_policy_.SetAllocated(retention_policy, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.retention_policy_.IsDefault()) {
    _impl_.retention_policy_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Person.retention_policy)
}

// bytes shard_id = 2;
inline void Person::clear_shard_id() {
  _impl_.shard_id_.ClearToEmpty();
}
inline const std::string& Person::shard_id() const {
  // @@protoc_insertion_point(field_get:Person.shard_id)
  return _internal_shard_id();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_shard_id(ArgT0&& arg0, ArgT... args) {
 
 _impl_.shard_id_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Person.shard_id)
}
inline std::string* Person::mutable_shard_id() {
  std::string* _s = _internal_mutable_shard_id();
  // @@protoc_insertion_point(field_mutable:Person.shard_id)
  return _s;
}
inline const std::string& Person::_internal_shard_id() const {
  return _impl_.shard_id_.Get();
}
inline void Person::_internal_set_shard_id(const std::string& value) {
  
  _impl_.shard_id_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_shard_id() {
  
  return _impl_.shard_id_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_shard_id() {
  // @@protoc_insertion_point(field_release:Person.shard_id)
  return _impl_.shard_id_.Release();
}
inline void Person::set_allocated_shard_id(std::string* shard_id) {
  if (shard_id != nullptr) {
    
  } else {
    
  }
  _impl_.shard_id_.SetAllocated(shard_id, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.shard_id_.IsDefault()) {
    _impl_.shard_id_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Person.shard_id)
}

// bytes database_name = 3;
inline void Person::clear_database_name() {
  _impl_.database_name_.ClearToEmpty();
}
inline const std::string& Person::database_name() const {
  // @@protoc_insertion_point(field_get:Person.database_name)
  return _internal_database_name();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_database_name(ArgT0&& arg0, ArgT... args) {
 
 _impl_.database_name_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Person.database_name)
}
inline std::string* Person::mutable_database_name() {
  std::string* _s = _internal_mutable_database_name();
  // @@protoc_insertion_point(field_mutable:Person.database_name)
  return _s;
}
inline const std::string& Person::_internal_database_name() const {
  return _impl_.database_name_.Get();
}
inline void Person::_internal_set_database_name(const std::string& value) {
  
  _impl_.database_name_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_database_name() {
  
  return _impl_.database_name_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_database_name() {
  // @@protoc_insertion_point(field_release:Person.database_name)
  return _impl_.database_name_.Release();
}
inline void Person::set_allocated_database_name(std::string* database_name) {
  if (database_name != nullptr) {
    
  } else {
    
  }
  _impl_.database_name_.SetAllocated(database_name, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.database_name_.IsDefault()) {
    _impl_.database_name_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Person.database_name)
}

// bytes measurement = 4;
inline void Person::clear_measurement() {
  _impl_.measurement_.ClearToEmpty();
}
inline const std::string& Person::measurement() const {
  // @@protoc_insertion_point(field_get:Person.measurement)
  return _internal_measurement();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_measurement(ArgT0&& arg0, ArgT... args) {
 
 _impl_.measurement_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Person.measurement)
}
inline std::string* Person::mutable_measurement() {
  std::string* _s = _internal_mutable_measurement();
  // @@protoc_insertion_point(field_mutable:Person.measurement)
  return _s;
}
inline const std::string& Person::_internal_measurement() const {
  return _impl_.measurement_.Get();
}
inline void Person::_internal_set_measurement(const std::string& value) {
  
  _impl_.measurement_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_measurement() {
  
  return _impl_.measurement_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_measurement() {
  // @@protoc_insertion_point(field_release:Person.measurement)
  return _impl_.measurement_.Release();
}
inline void Person::set_allocated_measurement(std::string* measurement) {
  if (measurement != nullptr) {
    
  } else {
    
  }
  _impl_.measurement_.SetAllocated(measurement, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.measurement_.IsDefault()) {
    _impl_.measurement_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Person.measurement)
}

// repeated bytes tsm_file = 5;
inline int Person::_internal_tsm_file_size() const {
  return _impl_.tsm_file_.size();
}
inline int Person::tsm_file_size() const {
  return _internal_tsm_file_size();
}
inline void Person::clear_tsm_file() {
  _impl_.tsm_file_.Clear();
}
inline std::string* Person::add_tsm_file() {
  std::string* _s = _internal_add_tsm_file();
  // @@protoc_insertion_point(field_add_mutable:Person.tsm_file)
  return _s;
}
inline const std::string& Person::_internal_tsm_file(int index) const {
  return _impl_.tsm_file_.Get(index);
}
inline const std::string& Person::tsm_file(int index) const {
  // @@protoc_insertion_point(field_get:Person.tsm_file)
  return _internal_tsm_file(index);
}
inline std::string* Person::mutable_tsm_file(int index) {
  // @@protoc_insertion_point(field_mutable:Person.tsm_file)
  return _impl_.tsm_file_.Mutable(index);
}
inline void Person::set_tsm_file(int index, const std::string& value) {
  _impl_.tsm_file_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set:Person.tsm_file)
}
inline void Person::set_tsm_file(int index, std::string&& value) {
  _impl_.tsm_file_.Mutable(index)->assign(std::move(value));
  // @@protoc_insertion_point(field_set:Person.tsm_file)
}
inline void Person::set_tsm_file(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.tsm_file_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:Person.tsm_file)
}
inline void Person::set_tsm_file(int index, const void* value, size_t size) {
  _impl_.tsm_file_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Person.tsm_file)
}
inline std::string* Person::_internal_add_tsm_file() {
  return _impl_.tsm_file_.Add();
}
inline void Person::add_tsm_file(const std::string& value) {
  _impl_.tsm_file_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:Person.tsm_file)
}
inline void Person::add_tsm_file(std::string&& value) {
  _impl_.tsm_file_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:Person.tsm_file)
}
inline void Person::add_tsm_file(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.tsm_file_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:Person.tsm_file)
}
inline void Person::add_tsm_file(const void* value, size_t size) {
  _impl_.tsm_file_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:Person.tsm_file)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
Person::tsm_file() const {
  // @@protoc_insertion_point(field_list:Person.tsm_file)
  return _impl_.tsm_file_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
Person::mutable_tsm_file() {
  // @@protoc_insertion_point(field_mutable_list:Person.tsm_file)
  return &_impl_.tsm_file_;
}

// bytes curr_file_path = 6;
inline void Person::clear_curr_file_path() {
  _impl_.curr_file_path_.ClearToEmpty();
}
inline const std::string& Person::curr_file_path() const {
  // @@protoc_insertion_point(field_get:Person.curr_file_path)
  return _internal_curr_file_path();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_curr_file_path(ArgT0&& arg0, ArgT... args) {
 
 _impl_.curr_file_path_.SetBytes(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Person.curr_file_path)
}
inline std::string* Person::mutable_curr_file_path() {
  std::string* _s = _internal_mutable_curr_file_path();
  // @@protoc_insertion_point(field_mutable:Person.curr_file_path)
  return _s;
}
inline const std::string& Person::_internal_curr_file_path() const {
  return _impl_.curr_file_path_.Get();
}
inline void Person::_internal_set_curr_file_path(const std::string& value) {
  
  _impl_.curr_file_path_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_curr_file_path() {
  
  return _impl_.curr_file_path_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_curr_file_path() {
  // @@protoc_insertion_point(field_release:Person.curr_file_path)
  return _impl_.curr_file_path_.Release();
}
inline void Person::set_allocated_curr_file_path(std::string* curr_file_path) {
  if (curr_file_path != nullptr) {
    
  } else {
    
  }
  _impl_.curr_file_path_.SetAllocated(curr_file_path, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.curr_file_path_.IsDefault()) {
    _impl_.curr_file_path_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Person.curr_file_path)
}

// int64 curr_file_margin = 7;
inline void Person::clear_curr_file_margin() {
  _impl_.curr_file_margin_ = int64_t{0};
}
inline int64_t Person::_internal_curr_file_margin() const {
  return _impl_.curr_file_margin_;
}
inline int64_t Person::curr_file_margin() const {
  // @@protoc_insertion_point(field_get:Person.curr_file_margin)
  return _internal_curr_file_margin();
}
inline void Person::_internal_set_curr_file_margin(int64_t value) {
  
  _impl_.curr_file_margin_ = value;
}
inline void Person::set_curr_file_margin(int64_t value) {
  _internal_set_curr_file_margin(value);
  // @@protoc_insertion_point(field_set:Person.curr_file_margin)
}

// int64 curr_file_head_offset = 8;
inline void Person::clear_curr_file_head_offset() {
  _impl_.curr_file_head_offset_ = int64_t{0};
}
inline int64_t Person::_internal_curr_file_head_offset() const {
  return _impl_.curr_file_head_offset_;
}
inline int64_t Person::curr_file_head_offset() const {
  // @@protoc_insertion_point(field_get:Person.curr_file_head_offset)
  return _internal_curr_file_head_offset();
}
inline void Person::_internal_set_curr_file_head_offset(int64_t value) {
  
  _impl_.curr_file_head_offset_ = value;
}
inline void Person::set_curr_file_head_offset(int64_t value) {
  _internal_set_curr_file_head_offset(value);
  // @@protoc_insertion_point(field_set:Person.curr_file_head_offset)
}

// int64 curr_file_tail_offset = 9;
inline void Person::clear_curr_file_tail_offset() {
  _impl_.curr_file_tail_offset_ = int64_t{0};
}
inline int64_t Person::_internal_curr_file_tail_offset() const {
  return _impl_.curr_file_tail_offset_;
}
inline int64_t Person::curr_file_tail_offset() const {
  // @@protoc_insertion_point(field_get:Person.curr_file_tail_offset)
  return _internal_curr_file_tail_offset();
}
inline void Person::_internal_set_curr_file_tail_offset(int64_t value) {
  
  _impl_.curr_file_tail_offset_ = value;
}
inline void Person::set_curr_file_tail_offset(int64_t value) {
  _internal_set_curr_file_tail_offset(value);
  // @@protoc_insertion_point(field_set:Person.curr_file_tail_offset)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Person_2eproto
