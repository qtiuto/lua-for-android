#ifndef HashMap_h
#define HashMap_h

#include "macros.h"
#include <string.h>
#include <stdint.h>
namespace std {

    const size_t DEFAULT_HASH_TABLE_SIZE = 16;
    const size_t DEFAULT_HASH_TABLE_LIMIT = 12;

    template< class Value,class Hash=std::hash<Value>,class Equal=std::equal_to<Value>> class HashSet {
    public:
        class HashNode {
            friend class HashSet<Value,Hash,Equal>;
            Value value;
            HashNode *next;	// This points to the next node in the list.
        public:
            HashNode( Value&& key ) : value(std::move(key)),next(nullptr) {}
        };
        typedef  HashNode* HashNodePtr;

        class Iterator{
            friend class HashSet<Value,Hash,Equal>;
            HashNodePtr ptr;
            size_t bucket;
            const HashSet& table;
        public:
            Iterator(HashNodePtr ptr,size_t bucket,const HashSet& table):ptr(ptr),bucket(bucket),table(table){}
            Value& operator*() const{return ptr->value;}
            Value* operator->() const{ return &ptr->value;}

            Iterator& operator++() {
                if(ptr->next!= nullptr){
                    ptr=ptr->next;
                } else{
                    ptr=table.findNextValidBucket(++bucket);
                }
                return *this;
            }

            Iterator operator++(int){
                Iterator c=*this;
                ++(*this);
                return c;
            }

            friend bool operator==(const Iterator& t,const Iterator& other){
                return t.ptr==other.ptr;
            }

            friend bool operator!=(const Iterator& t,const Iterator& other){
                return t.ptr!=other.ptr;
            }

            friend bool operator==(const Iterator& t,const Value* other){
                return (uintptr_t)t.ptr==(uintptr_t)other;
            }

            friend bool operator!=(const Iterator& t,const Value* other){
                return (uintptr_t)t.ptr!=(uintptr_t)other;
            }

            friend bool operator==(const Value* other,const Iterator& t){
                return (uintptr_t)t.ptr==(uintptr_t)other;
            }

            friend bool operator!=(const Value* other,const Iterator& t){
                return (uintptr_t)t.ptr!=(uintptr_t)other;
            }

        };

    private:
        HashNodePtr*   m_table;        // Hash table
        size_t         m_table_size;
        size_t         m_size;        // Count of nodes in the HashMap
        size_t         m_limit;

        template <typename K>
        static inline size_t hashIndex(const K& key ,size_t size) noexcept {return Hash()(key) & (size-1);}
        template <typename K>
        inline size_t hashIndex(const K& key ) const noexcept { return hashIndex(key,m_table_size);}

        HashNodePtr findNextValidBucket(size_t & bucket) const{
            auto table=m_table;
            for(size_t i=bucket,end=m_table_size;i<end;++i){
                HashNodePtr node = table[i];
                if(node){
                    bucket=i;
                    return node;
                }
            }
            return nullptr;
        }
        inline void ensureInit() {
            if(m_table_size == 0) {
                m_table =new HashNodePtr[DEFAULT_HASH_TABLE_SIZE]();
                m_table_size =DEFAULT_HASH_TABLE_SIZE;
                m_limit=DEFAULT_HASH_TABLE_LIMIT;
            }
        }
        template <typename K>
        inline HashNodePtr findNode(const K &key, const size_t index) const {
            HashNodePtr entry = m_table[index];
            while( entry != nullptr ) {
                if(Equal()(entry->value, key)) {
                    break;
                }
                entry = entry->next;
            }
            return entry;
        }
        inline HashNodePtr addNode(Value &&key, const size_t index) {
            HashNodePtr entry = new HashNode(std::move(key));
            entry->next=m_table[index];
            m_table[index] = entry;
            if(++m_size > m_limit){
                rehash();
            }
            return entry;
        }
        void rehash(){
            size_t newSize=m_table_size<<1;
            HashNodePtr* buckets=new HashNodePtr[newSize]();
            HashNodePtr* old = m_table;
            HashNodePtr* current=m_table;
            HashNodePtr item,next;
            for (size_t i = m_table_size-1; i !=-1; i--) {
                for (item = current[i]; item != NULL; item = next) {
                    size_t index = hashIndex(item->value, newSize);
                    next = item->next;
                    item->next = buckets[index];
                    buckets[index] = item;
                }
            }
            m_table=buckets;
            m_table_size=newSize;
            m_limit <<=1;
            delete [] old;
        }
        inline void free(){
            if(m_table_size==0)
                return;
            clear();
            delete[] m_table;
            m_table_size = 0;
        }
        static inline bool isPowerOfTwo(size_t n) {
            return ((n & (n -1)) == 0);
        }
        static inline size_t binaryCeil(size_t i) {
            i |= (i >> 1);
            i |= (i >> 2);
            i |= (i >> 4);
            i |= (i >> 8);
            i |= (i >> 16);
#if defined(__LP64__)
            i |= (i >> 32);
#endif
            return i + 1;
        }
    public:
        HashSet():m_size(0),m_table_size(0){}
        HashSet( size_t tableSize) : // Best to keep table size a power of two.
                m_table_size( tableSize ),
                m_size( 0 ) {
            if( m_table_size ==0 ) {
                m_table_size = DEFAULT_HASH_TABLE_SIZE;
                m_limit=DEFAULT_HASH_TABLE_LIMIT;
            } else if(!isPowerOfTwo(tableSize)){
                m_table_size=binaryCeil(tableSize);
                m_limit=(m_table_size*3)>>2;
            }
            m_table = new HashNodePtr[m_table_size]();
        }
        HashSet(std::initializer_list<Value> keys):HashSet((keys.size()*3)>>2){
            for(auto&& v:keys){
                insert(std::move(v));
            }
        }

        HashSet(HashSet&& other):m_table(other.m_table),m_table_size(other.m_table_size),m_size(other.m_size),m_limit(other.m_limit){
            other.m_table_size=0;
            other.m_table= nullptr;
        }
        HashSet(const HashSet& other):m_size(other.m_size),m_table_size(other.m_table_size),m_limit(other.m_limit){
            m_table=new HashNodePtr[m_table_size]();
            for(auto&& v:other){
                insert(v);
            }
        }

        HashSet& operator=(HashSet && other){
            free();
            m_table=other.m_table;
            m_table_size=other.m_table_size;
            m_size=other.m_size;
            m_limit=other.m_limit;
            other.m_table_size=0;
            other.m_table= nullptr;
            return *this;
        }

        HashSet& operator=(const HashSet & other){
            free();
            m_table_size=other.m_table_size;
            m_size=other.m_size;
            m_limit=other.m_limit;
            m_table=new HashNodePtr[m_table_size]();
            for(auto&& v:other){
                insert(v);
            }
            return *this;
        }

        ~HashSet( ) {
            free();
        }

        inline size_t size()  const noexcept { return m_size; }
        inline bool isEmpty() const noexcept { return m_size == 0; }


        void clear() noexcept {
            for( size_t ii = m_table_size-1; ii !=-1 ; ii-- ) {
                HashNodePtr entry = m_table[ii];
                if( entry != nullptr ) {
                    do {
                        HashNodePtr next = entry->next;
                        delete entry;
                        entry = next;
                    } while( entry != 0 );
                    m_table[ii] = 0;
                }
            }
            m_size = 0;
        }
        Value* find(const Value& key ) const noexcept{
            return find_unique(key);
        }

        template <typename K>
        Value* find_unique(const K& key ) const noexcept {
            if(m_table_size==0)
                return nullptr;
            const size_t index = hashIndex( key );
            return &findNode(key, index)->value;
        }

        Iterator begin() const {
            if(size()==0)
                return Iterator(nullptr,0,*this);
            size_t bucket= 0;
            return Iterator(findNextValidBucket(bucket),bucket,*this);
        }

        inline Iterator end()const{
            return Iterator(nullptr,0,*this);
        }


        inline std::pair<Value*,bool> insert(const Value& key ){
            return insert(std::move(Value(key)));
        }
        template <typename V>
        std::pair<Value*,bool> emplace(V&& value ){
            return insert(Value(std::move(value)));
        }

        std::pair<Value*,bool> insert(Value&& key ) {
            ensureInit();
            const size_t index = hashIndex( key );
            HashNodePtr entry = findNode(key,index);
            if (entry)
                return std::make_pair(&entry->value, false);
            entry = addNode(std::move(key),index);
            return std::make_pair(&entry->value, true);
        }

        void erase(const Iterator& iter){
            if(m_table_size==0||iter.ptr== nullptr)
                return;
            auto index=iter.bucket;
            HashNodePtr entry=m_table[index],previous= nullptr;
            while( entry != nullptr ) {
                if(entry==iter.ptr) {
                    if( previous == nullptr ) {
                        m_table[index] = entry->next;
                    } else {
                        previous->next = entry->next;
                    }
                    delete entry;
                    m_size--;
                    return;
                } else {
                    previous = entry;
                    entry = entry->next;
                }
            }
        }



        inline void erase(const Value& key ){
            erase_unique(key);
        }
        template <typename V>
        void erase_unique(const V& key ) {
            if(m_table_size==0)
                return;
            const size_t index = hashIndex( key );
            HashNodePtr previous = nullptr;
            HashNodePtr entry    = m_table[index];
            while( entry != nullptr ) {
                if(Equal()(entry->value,key) ) {
                    if( previous == nullptr ) {
                        m_table[index] = entry->next;
                    } else {
                        previous->next = entry->next;
                    }
                    delete entry;
                    m_size--;
                    return;
                } else {
                    previous = entry;
                    entry = entry->next;
                }
            }
        }

    };
    template <class K, class V, class Equal>
    struct HEqual {
        bool operator()(const std::pair<K, V>& l, const std::pair<K, V>& r) const noexcept {
            return Equal()(l.first, r.first);
        }
        bool operator()(const std::pair<K, V>& l, const K& r) const noexcept {
            return Equal()(l.first, r);
        }
        bool operator()(const K& l, const std::pair<K, V>& r) const noexcept {
            return Equal()(l, r.first);
        }
        bool operator()(const K& l, const K& r) const noexcept {
            return Equal()(l, r);
        }
    };
    template <class K, class V, class Hasher>
    struct HHaser {
        size_t operator()(const std::pair<K, V>& l) const noexcept {
            return Hasher()(l.first);
        }
        size_t operator()(const K& l) const noexcept {
            return Hasher()(l);
        }
    };
    template< class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
    class HMap:public HashSet<std::pair<Key,T>,HHaser<Key,T,Hash>,HEqual<Key,T,Equal>>{
    public:
        typedef HashSet<std::pair<Key,T>,HHaser<Key,T,Hash>,HEqual<Key,T,Equal>> Super;
        typedef std::pair<Key,T> ValueType;
        typedef typename Super::Iterator Iterator;
        HMap():Super(){};
        HMap(size_t cap):Super(cap){};
        HMap(HMap&& other):Super(std::move(other)){
        }
        HMap(const HMap& other):Super(other){
        }

        HMap& operator=(HMap && other){
            Super::operator=(std::move(other));
            return *this;
        }

        HMap& operator=(const HMap & other){
            Super::operator=(other);
            return *this;
        }

        template <typename K, typename V>
        std::pair<ValueType*, bool> emplace(K&& key, V&& value) {
            return Super::insert(std::make_pair(Key(std::forward<K>(key)), T(std::forward<V>(value))));
        }
        ValueType* find ( Key&& key){
            return find((const Key&)key);
        }
        ValueType* find (const Key& key) {
            return Super::find_unique(key);
        }

        T& operator[](const Key& key) {
            return operator[](Key(key));
        }

        T& operator[](Key&& key) {
            auto&&p = std::make_pair(move(key), T());
            return Super::insert(move(p)).first->second;
        }

        void erase(const Key& key){
            Super::erase_unique(key);
        }

        inline void erase(const Iterator& iter){
            Super::erase(iter);
        }
    };
}   // namespace std


#endif /* HashMap_h */