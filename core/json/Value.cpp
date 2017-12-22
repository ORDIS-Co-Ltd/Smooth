#include <smooth/core/json/Json.h>
#include <smooth/core/util/make_unique.h>

using namespace smooth::core::util;

namespace smooth
{
    namespace core
    {
        namespace json
        {
            Value::Value(cJSON* src)
            {
                data = src;
            }

            Value::Value(const std::string& src)
            {
                data = cJSON_Parse(src.c_str());
            }

            Value::Value(cJSON* parent, const char* key_name, cJSON* object)
            {
                this->parent = parent;
                key = key_name;
                data = object;
            }

            Value Value::operator[](const std::string& key)
            {
                auto item = cJSON_GetObjectItemCaseSensitive(data, key.c_str());

                if(item)
                {
                    return Value(data, key.c_str(), item);
                }
                else
                {
                    auto new_item = cJSON_CreateObject();
                    cJSON_AddItemToObject(data, key.c_str(), new_item);
                    return Value(data, key.c_str(), new_item);
                }
            }

            Value& Value::operator=(const std::string& s)
            {
                if(parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = cJSON_CreateString(s.c_str());
                }
                else
                {
                    data = cJSON_CreateString(s.c_str());
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, key.c_str(), data);
                }

                return *this;
            }

            Value& Value::operator=(int value)
            {
                if(parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = cJSON_CreateNumber(value);
                }
                else
                {
                    data = cJSON_CreateNumber(value);
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, key.c_str(), data);
                }

                return *this;
            }

            Value& Value::operator=(double value)
            {
                if(parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = cJSON_CreateNumber(value);
                }
                else
                {
                    data = cJSON_CreateNumber(value);
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, key.c_str(), data);
                }

                return *this;
            }

            Value& Value::operator=(const Value& other)
            {
                if(this != &other)
                {
                    if(parent == nullptr)
                    {
                        // We're the root, replace it all.
                        cJSON_Delete(data);
                        data = other.data;
                    }
                    else
                    {
                        cJSON_ReplaceItemInObjectCaseSensitive(parent, key.c_str(), other.data);
                    }
                }

                return *this;
            }

            bool Value::operator==(const std::string& s) const
            {
                return cJSON_IsString(data) && s == data->valuestring;
            }

            bool Value::operator==(int value) const
            {
                return cJSON_IsNumber(data) && value == data->valueint;
            }

            bool Value::operator==(double value) const
            {
                return cJSON_IsNumber(data) && value == data->valuedouble;
            }

            Value::operator std::string() const
            {
                return cJSON_IsString(data) ? data->valuestring : "";
            }

            Value::operator int() const
            {
                return cJSON_IsNumber(data) ? data->valueint : 0;
            }

            Value::operator double() const
            {
                return cJSON_IsNumber(data) ? data->valuedouble : 0;
            }
        }
    }
}