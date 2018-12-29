package com.oslorde.luadroid.set;

abstract class BaseSet<N extends BaseNode<N>,K>{
    private int size;
    private int cap;
    private int limit;
    private N[] nodes;
    BaseSet(){
        this(8);
    }

    BaseSet(int expectedSize){
        init(expectedSize);
    }

    private void init(int expectedSize) {
        int trueCap=(expectedSize<<2)/3;
        if(SetUtils.notPowerOfTwo(trueCap) ||expectedSize==0)
            trueCap= SetUtils.binaryCeil(trueCap);
        limit=(trueCap*3)>>2+1;
        cap=trueCap;
    }

    protected int hashKey(K key){
        return key.hashCode();
    }

    protected int hashNode(N node){
        return node.hashCode();
    }

    private int hashKey(K key,int cap){
        return hashKey(key)&cap-1;
    }

    private int hashNode(N node,int cap){
        return hashNode(node)&cap-1;
    }

    protected boolean equals(N node,K key){
        return node.equals(key);
    }
    private void rehash(){
        int newCap=cap<<1;
        //noinspection unchecked
        N[] newNodes= (N[]) new BaseNode[newCap];
        N next;
        for (N node:nodes){
            for(;node!=null;node=next){
                int index=hashNode(node,newCap);
                next=node.next;
                node.next=newNodes[index];
                newNodes[index]=node;
            }
        }
        nodes=newNodes;
        cap=newCap;
        limit=(newCap*3)>>2;
    }
    public boolean add(K key){
        int index = hashKey(key,cap);
        if(nodes==null){
            //noinspection unchecked
            nodes= (N[]) new BaseNode[cap];
        }
        N start = nodes[index];
        for (N node = start; node != null; node = node.next) {
            if (equals(node,key)) {
                return false;
            }
        }
        nodes[index] =onNewNode(key,start);
        if (++size > limit) rehash();
        return true;
    }

    public boolean remove(K key){
        int index = hashKey(key,cap);
        if(nodes==null){
            //noinspection unchecked
            return false;
        }
        N start = nodes[index];
        N prev=null;
        for (N node = start; node != null; node = node.next) {
            if (equals(node,key)) {
                if(prev==null) nodes[index]=node.next;
                else prev.next=node.next;
            }
            prev=node;
        }
        return false;
    }

    abstract N onNewNode(K key,N next);

    public boolean contains(K key){
        int index = hashKey(key,cap);
        if(nodes==null){
            return false;
        }
        N start = nodes[index];
        for (N node = start; node != null; node = node.next) {
            if (equals(node,key)) {
                return true;
            }
        }
        return false;
    }
}
