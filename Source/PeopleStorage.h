#pragma once

#include "PeopleLoadData.h"
#include "Person.h"

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

		const std::vector<Person>& GetPeople() const { return m_People; }
	private:
		std::vector<Person> m_People;
		std::string m_PersonFileExtension = ".ppnw.json";
	};
}
