#include "Json.h"

namespace Loopie {

#pragma region Json

    JsonData Json::ReadFromString(const std::string& data)
    {
        JsonData jsonData;
        jsonData.m_data = json::parse(data);
        jsonData.m_empty = jsonData.m_data.is_discarded();
        return jsonData;
    }

    JsonData Json::ReadFromFile(const std::filesystem::path& filePath)
    {
        JsonData jsonData;
        std::ifstream ifs(filePath);
        jsonData.m_data = json::parse(ifs);
        jsonData.m_empty = jsonData.m_data.is_discarded();
        return jsonData;
    }

    bool Json::WriteToFile(const std::filesystem::path& filePath, const JsonData& jsonData, int indent)
    {
        std::ofstream ofs(filePath);
        if (!ofs.is_open()) return false;
        ofs << jsonData.m_data.dump(indent);
        return true;
    }

#pragma endregion


#pragma region JsonNode

    JsonNode::JsonNode(json* node) : m_node(node) {}
    JsonNode::JsonNode(json* node, json* parentNode) : m_node(node), m_parentNode(parentNode) {}

    void JsonNode::Reset() {
        m_node = nullptr;
        m_parentNode = nullptr;
    }

    JsonNode JsonNode::Child(const std::string& keyPath) const
    {
        if (!IsValid())
            return JsonNode();

        json* node = m_node;
        json* parentNode = m_parentNode;

        std::stringstream ss(keyPath);
        std::string token;
        while (std::getline(ss, token, '.')) {
            if (!node->contains(token))
                return JsonNode();
            parentNode = node;
            node = &((*node)[token]);
        }

        return JsonNode(node, parentNode);
    }

    bool JsonNode::Contains(const std::string& keyPath) const
    {
        if (keyPath.empty() || !IsValid())
            return false;

        JsonNode node = Child(keyPath);
        return node.IsValid();
    }

    std::string JsonNode::ToString(int indent) const
    {
        if (!IsValid())
            return "{}";
        return m_node->dump(indent);
    }

    bool JsonNode::Remove(const std::string& keyPath) {
        if (keyPath.empty())
            return false;

        auto pos = keyPath.find_last_of('.');
        std::string parentPath = (pos == std::string::npos) ? "" : keyPath.substr(0, pos);
        std::string key = (pos == std::string::npos) ? keyPath : keyPath.substr(pos + 1);

        JsonNode node = Child(parentPath);
        if (!node.IsValid())
            return false;

        if (!node.m_node->contains(key))
            return false;

        node.m_node->erase(key);
        return true;
    }

    bool JsonNode::RemoveSelf(const std::string& name)
    {
        if (name.empty())
            return false;

        if (m_parentNode == nullptr || !IsValid())
            return false;

        if (!m_parentNode->contains(name))
            return false;

        m_parentNode->erase(name);
        return true;
    }

    JsonNode JsonNode::CreateObjectField(const std::string& keyPath) {
        return CreateField(keyPath, json::object());
    }

    JsonNode JsonNode::CreateArrayField(const std::string& keyPath) {
        return CreateField(keyPath, json::array());
    }

    bool JsonNode::RemoveArrayElement(const std::string& keyPath, unsigned int index) {
        if (keyPath.empty())
            return RemoveArrayElement(index);

        JsonNode node = Child(keyPath);
        if (!node.IsArray())
            return false;

        unsigned int size = node.Size();
        if (size == 0 || size <= index)
            return false;
        node.m_node->erase(node.m_node->begin() + index);
        return true;
    }

    bool JsonNode::RemoveArrayElement(unsigned int index) {
        if (!IsArray())
            return false;

        unsigned int size = Size();
        if (size == 0 || size <= index)
            return false;
        m_node->erase(m_node->begin() + index);
        return true;
    }

    bool JsonNode::ClearArray(const std::string& keyPath) {
        if (keyPath.empty())
            return ClearArray();

        JsonNode node = Child(keyPath);
        if (!node.IsArray())
            return false;

        node.m_node->clear();
        return true;
    }

    bool JsonNode::ClearArray() {
        if (!IsArray())
            return false;

        m_node->clear();
        return true;
    }

#pragma endregion


#pragma region JsonData

    bool JsonData::ToFile(const std::filesystem::path& filePath, int indent)
    {
        return Json::WriteToFile(filePath, *this, indent);
    }

#pragma endregion
}
