#pragma once

#include "PeopleLoadData.h"
#include "Person.h"

#include <unordered_map>

namespace PeopleNetwork
{
	class PeopleStorage
	{
	public:
		PeopleStorage() = default;
		PeopleStorage(const PeopleStorage&) = default;
		PeopleStorage& operator=(const PeopleStorage&) = default;
		
		void Reset();
		void Set(const std::vector<Person>& people);

		bool LoadFromConfig(const PeopleLoadData& cfg, bool reset = false);
		bool LoadFromDirRoot(std::string_view root, const std::vector<std::string>& excludes = {}, bool reset = false);
		bool LoadFromFiles(const std::vector<std::string>& files, bool reset = false);
		bool LoadFromFile(std::string_view file);
		void Upload(const Person& person);

		void SetPersonFileExtension(std::string_view extension);
		const std::string& GetPersonFileExtension() const { return m_PersonFileExtension; }

		// Will be used for all Person::PopulateFromJson calls
		void SetWrapContentTolerance(size_t wrapContentTolerance);
		size_t GetWrapContentTolerance() const { return m_WrapContentTolerance; }

		const std::unordered_map<PRSHANDLE, Person>& GetPeople() const { return m_People; }
	private:
		std::unordered_map<PRSHANDLE, Person> m_People;
		std::string m_PersonFileExtension = ".ppnw.json";
		size_t m_WrapContentTolerance = DO_NOT_WRAP_CONTENT;
	};
}
