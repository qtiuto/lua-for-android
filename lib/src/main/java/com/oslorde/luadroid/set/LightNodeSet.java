package com.oslorde.luadroid.set;

public class LightNodeSet<N extends BaseNode<N>>extends BaseSet<N,N> {
    public LightNodeSet(){}

    @Override
    N onNewNode(N key, N next) {
        key.next=next;
        return key;
    }
}
