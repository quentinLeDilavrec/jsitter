package jsitter.impl

import jsitter.api.*
import jsitter.interop.JSitter
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ConcurrentMap

typealias TSSymbol = Int

class TSLanguage<T : NodeType>(
        val languagePtr: Ptr,
        override val name: String,
        override val rootNodeType: T,
        val registry: ConcurrentMap<String, NodeType> = ConcurrentHashMap(),
        val nodeTypesCache: ConcurrentMap<TSSymbol, NodeType> = ConcurrentHashMap()) : Language<T> {

  fun getNodeType(tsSymbol: TSSymbol): NodeType =
    nodeTypesCache.computeIfAbsent(tsSymbol) { symbol ->
      if (symbol.toInt() == -1) {
          Error
      }
      else {
        val name: String? = JSitter.getSymbolName(languagePtr, symbol)
        val isTerminal: Boolean = JSitter.isTerminal(languagePtr, symbol)
        val nodeType = registry.computeIfAbsent(name) { name ->
          if (isTerminal) {
              Terminal(name)
          }
          else {
              NodeType(name)
          }
        }
        nodeType.id = symbol.toInt()
        nodeType
      }
    }

  fun getNodeTypeSymbol(nodeType: NodeType): TSSymbol =
    if (nodeType.initialized) {
      nodeType.id
    }
    else {
      val symbol: TSSymbol = JSitter.getSymbolByName(languagePtr, nodeType.name)
      nodeType.id = symbol
      nodeType.initialized = true
      symbol
    }

  override fun nodeType(name: String): NodeType = registry[name]!!

  override fun parser(): Parser<T> {
    val cancellationFlagPtr = SubtreeAccess.unsafe.allocateMemory(8)
    return TSParser(parserPtr = JSitter.newParser(languagePtr, cancellationFlagPtr),
            language = this,
            nodeType = rootNodeType,
            cancellationFlagPtr = cancellationFlagPtr)
  }

  override fun register(nodeType: NodeType) {
    registry[nodeType.name] = nodeType
  }
}