#pragma once
#include "management.h"

namespace kagami {
  namespace management {
    list<ObjectContainer> &GetContainerPool() {
      static list<ObjectContainer> base;
      return base;
    }

    ObjectContainer &GetConstantBase() {
      static ObjectContainer base;
      return base;
    }

    vector<Interface> &GetInterfaceBase() {
      static vector<Interface> base;
      return base;
    }

    map<GenericToken, Interface> &GetGenericInterfaceBase() {
      static map<GenericToken, Interface> base;
      return base;
    }

    Object *FindObject(string id) {
      Object *object = nullptr;
      size_t count = GetContainerPool().size();
      list<ObjectContainer> &base = GetContainerPool();
      ObjectContainer &const_base = GetConstantBase();

      while (!base.empty() && count > 0) {
        object = base[count - 1].Find(id);
        if (object != nullptr) {
          break;
        }
        count--;
      }

      //TODO:constant write lock
      if (object == nullptr) {
        object = const_base.Find(id);
      }

      return object;
    }

    ObjectContainer &GetCurrentContainer() {
      return GetContainerPool().back();
    }

    Object *CreateObject(string id, Object &object) {
      ObjectContainer &base = GetContainerPool().back();
      ObjectContainer &const_base = GetConstantBase();

      if (base.Find(id) != nullptr && const_base.Find(id) != nullptr) {
        return nullptr;
      }
      base.Add(id, object);
      auto result = base.Find(id);
      return result;
    }

    Object *CreateObject(string id, Object &&object) {
      return CreateObject(id, object);
    }

    Object *CreateConstantObject(string id, Object &object) {
      ObjectContainer &base = GetConstantBase();

      if (base.Find(id) != nullptr) return nullptr;

      base.Add(id, object);
      auto result = base.Find(id);
      return result;
    }

    Object *CreateConstantObject(string id, Object &&object) {
      return CreateConstantObject(id, object);
    }

    ObjectContainer &CreateContainer() {
      auto &base = GetContainerPool();
      base.push_back(std::move(ObjectContainer()));
      return GetContainerPool().back();
    }

    bool DisposeManager() {
      auto &base = GetContainerPool();
      if (!base.empty()) { base.pop_back(); }
      return base.empty();
    }

    bool HasTailTokenRequest(GenericToken token) {
      return (token == kTokenIf || token == kTokenWhile || token == kTokenCase);
    }

    void CreateInterface(Interface temp) {
      GetInterfaceBase().emplace_back(temp);
    }

    void CreateInterface(std::initializer_list<Interface> &&rhs) {
      auto &base = GetInterfaceBase();
      for (auto &unit : rhs) {
        base.emplace_back(unit);
      }
    }

    void CreateGenericInterface(Interface temp) {
      GetGenericInterfaceBase().insert(pair<GenericToken, Interface>(
        temp.GetTokenEnum(), temp));
    }

    Interface GetGenericInterface(GenericToken token) {
      auto &base = GetGenericInterfaceBase();
      map<GenericToken, Interface>::iterator it = base.find(token);
      if (it != base.end()) return it->second;
      return Interface();
    }


    Interface Order(string id, string type, int size) {
      GenericToken basicOpCode = util::GetGenericToken(id);
      if (basicOpCode != kTokenNull) {
        return GetGenericInterface(basicOpCode);
      }

      vector<Interface> &base = GetInterfaceBase();
      Interface result;
      bool ignore_type = (type == kTypeIdNull);
      //TODO:rewrite here
      for (auto &unit : base) {
        bool typeChecking = (ignore_type || type == unit.GetTypeDomain());
        bool sizeChecking = (size == -1 || size == int(unit.GetParamSize()));
        if (id == unit.GetId() && typeChecking && sizeChecking) {
          result = unit;
          break;
        }
      }
      return result;
    }

    namespace type {
      map <string, ObjectCopyingPolicy> &GetPlannerBase() {
        static map<string, ObjectCopyingPolicy> base;
        return base;
      }

      shared_ptr<void> GetObjectCopy(Object &object) {
        if (object.GetConstructorFlag()) {
          return object.Get();
        }

        shared_ptr<void> result = nullptr;
        const auto option = object.GetTypeId();
        const auto it = GetPlannerBase().find(option);

        if (it != GetPlannerBase().end()) {
          result = it->second.CreateObjectCopy(object.Get());
        }
        return result;
      }

      string GetMethods(string name) {
        string result;
        const auto it = GetPlannerBase().find(name);

        if (it != GetPlannerBase().end()) {
          result = it->second.GetMethods();
        }
        return result;
      }

      void AddTemplate(string name, ObjectCopyingPolicy temp) {
        GetPlannerBase().insert(pair<string, ObjectCopyingPolicy>(name, temp));
      }
    }
  }
}