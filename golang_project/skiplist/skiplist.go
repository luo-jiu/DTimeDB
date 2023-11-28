package main

import (
	"fmt"
	"math/rand"
)

const maxLevel = 32     // 最大级别
const Probability = 0.5 // 层数增长的概率

type SkipList struct {
	head *node
}

type node struct {
	nexts    []*node
	key, val int
}

// Get 根据 key 读取 val，第二个 bool flag 反映 key 在 skiplist 中是否存在
func (s *SkipList) Get(key int) (int, bool) {
	// 根据 key 尝试检索对应的 node，如果 node 存在，则返回对应的 val
	if _node := s.search(key); _node != nil {
		return _node.val, true
	}

	return -1, false
}

// 从跳表中检索 key 对应的 node
func (s *SkipList) search(key int) *node {
	// 每次检索从头部出发
	move := s.head
	// 每次检索从最大高度出发，直到来到首层
	for level := len(s.head.nexts) - 1; level >= 0; level-- {
		// 在每一层中持续向右遍历，直到下一个节点不存在或者 key 值大于等于 key
		for move.nexts[level] != nil && move.nexts[level].key < key {
			move = move.nexts[level]
		}
		// 如果 key 值相等，则找到了目标直接返回
		if move.nexts[level] != nil && move.nexts[level].key == key {
			return move.nexts[level]
		}

		// 当前层没找到目标，则层数减 1，继续向下
	}

	// 遍历完所有层数，都没有找到目标，返回 nil
	return nil
}

// roll 骰子，决定一个待插入的新节点在 skiplist 中最高层对应的 index
func (s *SkipList) roll() int {
	level := 0
	for rand.Float32() < Probability && level < maxLevel {
		level++
	}
	return level
}

// Put 将 key-val 对加入 skipList
func (s *SkipList) Put(key, val int) {
	// 假如 kv对已存在，则直接对值进行更新并返回
	if _node := s.search(key); _node != nil {
		_node.val = val
		return
	}

	// roll 出新节点的高度
	level := s.roll()

	// 新节点高度超出跳表最大高度，则需要对高度进行补齐
	for len(s.head.nexts)-1 < level {
		s.head.nexts = append(s.head.nexts, nil)
	}

	// 创建出新的节点
	newNode := node{
		key:   key,
		val:   val,
		nexts: make([]*node, level+1),
	}

	// 从头节点的最高层出发
	move := s.head
	for level := level; level >= 0; level-- {
		// 向右遍历，直到右侧节点不存在或者 key 值大于 key
		for move.nexts[level] != nil && move.nexts[level].key < key {
			move = move.nexts[level]
		}

		// 调整指针关系，完成新节点的插入
		newNode.nexts[level] = move.nexts[level]
		move.nexts[level] = &newNode
	}
}

// Del 根据 key 从跳表中删除对应的节点
func (s *SkipList) Del(key int) {
	// 如果 kv 对不存在，则无需删除直接返回
	if _node := s.search(key); _node == nil {
		return
	}

	// 从头节点的最高层出发
	move := s.head
	for level := len(s.head.nexts) - 1; level >= 0; level-- {
		// 向右遍历，直到右侧节点不存在或者 key 值大于等于 key
		for move.nexts[level] != nil && move.nexts[level].key < key {
			move = move.nexts[level]
		}

		// 右侧节点不存在或者 key 值大于 target，则直接跳过
		if move.nexts[level] == nil || move.nexts[level].key > key {
			continue
		}

		// 走到此处意味着右侧节点的 key 值必然等于 key，则调整指针引用
		move.nexts[level] = move.nexts[level].nexts[level]
	}

	// 对跳表的最大高度进行更新
	var dif int
	// 倘若某一层已经不存在数据节点，高度需要递减
	for level := len(s.head.nexts) - 1; level > 0 && s.head.nexts[level] == nil; level-- {
		dif++
	}
	s.head.nexts = s.head.nexts[:len(s.head.nexts)-dif]
}

// Range 找到 skipList 当中 ≥ start，且 ≤ end 的 kv 对
func (s *SkipList) Range(start, end int) [][2]int {
	// 首先通过 ceiling 方法，找到 skipList 中 key 值大于等于 start 且最接近于 start 的节点 ceilNode
	ceilNode := s.ceiling(start)
	// 如果不存在，直接返回
	if ceilNode == nil {
		return [][2]int{}
	}

	// 从 ceilNode 首层出发向右遍历，把所有位于 [start,end] 区间内的节点统统返回
	var res [][2]int
	for move := ceilNode; move != nil && move.key <= end; move = move.nexts[0] {
		res = append(res, [2]int{move.key, move.val})
	}
	return res
}

// 找到 key 值大于等于 target 且 key 值最接近于 target 的节点
func (s *SkipList) ceiling(target int) *node {
	move := s.head

	// 自上而下，找到 key 值小于 target 且最接近 target 的 kv 对
	for level := len(s.head.nexts) - 1; level >= 0; level-- {
		for move.nexts[level] != nil && move.nexts[level].key < target {
			move = move.nexts[level]
		}
		// 如果 key 值等于 target 的 kv 对存在，则直接返回
		if move.nexts[level] != nil && move.nexts[level].key == target {
			return move.nexts[level]
		}
	}

	// 此时 move 已经对应于在首层 key 值小于 key 且最接近于 key 的节点，其右侧第一个节点即为所寻找的目标节点
	return move.nexts[0]
}

// Ceiling 找到 skipList 中，key 值大于等于 target 且最接近于 target 的 key-value 对
func (s *SkipList) Ceiling(target int) ([2]int, bool) {
	if ceilNode := s.ceiling(target); ceilNode != nil {
		return [2]int{ceilNode.key, ceilNode.val}, true
	}
	return [2]int{}, false
}

// Floor 找到 skipList 中，key 值小于等于 target 且最接近于 target 的 key-value 对
func (s *SkipList) Floor(target int) ([2]int, bool) {
	// 引用 floor 方法，取 floorNode 值进行返回
	if floorNode := s.floor(target); floorNode != nil {
		return [2]int{floorNode.key, floorNode.val}, true
	}

	return [2]int{}, false
}

// 找到 key 值小于等于 target 且 key 值最接近于 target 的节点
func (s *SkipList) floor(target int) *node {
	move := s.head

	// 自上而下，找到 key 值小于 target 且最接近 target 的 kv 对
	for level := len(s.head.nexts) - 1; level >= 0; level-- {
		for move.nexts[level] != nil && move.nexts[level].key < target {
			move = move.nexts[level]
		}
		// 如果 key 值等于 target 的 kv对存在，则直接返回
		if move.nexts[level] != nil && move.nexts[level].key == target {
			return move.nexts[level]
		}
	}

	// move 是首层中 key 值小于 target 且最接近 target 的节点，直接返回 move 即可
	return move
}

func main() {
	head := node{}
	skipList := SkipList{
		head: &head,
	}
	skipList.Put(1, 101)
	skipList.Put(3, 103)
	skipList.Put(5, 105)
	skipList.Put(7, 107)
	skipList.Put(9, 109)
	skipList.Put(2, 102)
	skipList.Put(4, 104)
	skipList.Put(6, 106)
	skipList.Put(8, 108)
	skipList.Put(10, 1010)

	fmt.Print(skipList.Range(1, 10))
}
