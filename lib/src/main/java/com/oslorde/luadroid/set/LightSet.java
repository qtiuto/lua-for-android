package com.oslorde.luadroid.set;

public class LightSet<N> extends BaseSet<LightSet.Node<N>,N> {
    static protected class Node<N>extends BaseNode<Node<N>> {
        public N key;
        Node( N key,Node<N> next){
            this.next=next;
            this.key=key;
        }

        @Override
        public int hashCode() {
            return key.hashCode();
        }
    }
    public LightSet(){}
    public LightSet(int s){
        super(s);
    }
    @Override
    protected boolean equals(Node<N> node, N key) {
        return node.key.equals(key);
    }

    @Override
    Node<N> onNewNode(N key, Node<N> next) {
        return new Node<>(key,next);
    }


    public void addAll(N[] keys){
        for (N e:keys){
            add(e);
        }
    }
}
