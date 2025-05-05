#include "PeopleStorage.h"

#include "FileUtil.h"

namespace PeopleNetwork
{
	void PeopleStorage::Reset()
	{
		m_People.clear();
	}
	
	void PeopleStorage::Set(const std::vector<Person>& people)
	{
		Reset();
		for (const Person& person : people)
		{
			m_People[person.Handle] = person;
		}
	}
	
	bool PeopleStorage::LoadFromConfig(const PeopleLoadData& cfg, bool reset)
	{
		switch (cfg.LdRule)
		{
		case PeopleLoadData::LoadRule::LoadAll:      return LoadFromDirRoot(cfg.LoadAllRoot, cfg.LoadAllExcludes, reset);
		case PeopleLoadData::LoadRule::LoadSpecific: return LoadFromFiles(cfg.LoadSpecificFiles, reset);
		}

		return false;
	}

	bool PeopleStorage::LoadFromDirRoot(std::string_view root, const std::vector<std::string>& excludes, bool reset)
	{
		if (reset)
		{
			Reset();
		}
		for (const std::filesystem::path& file : std::filesystem::recursive_directory_iterator(root))
		{
			std::string fstr = std::filesystem::relative(file).string();
			std::replace(fstr.begin(), fstr.end(), '\\', '/');
			if (fstr.ends_with(m_PersonFileExtension) && std::find(excludes.begin(), excludes.end(), fstr) == excludes.end())
			{
				if (!LoadFromFile(fstr))
				{
					return false;
				}
			}
		}
		return true;
	}
	
	bool PeopleStorage::LoadFromFiles(const std::vector<std::string>& files, bool reset)
	{
		if (reset)
		{
			Reset();
		}
		for (const std::string_view& file : files)
		{
			if (!LoadFromFile(file))
			{
				return false;
			}
		}
		return true;
	}
	
	bool PeopleStorage::LoadFromFile(std::string_view file)
	{
		Person person;
		if (!person.PopulateFromJson(file, m_WrapContentTolerance))
		{
			return false;
		}
		m_People[person.Handle] = person;
		return true;
	}

	void PeopleStorage::Upload(const Person& person)
	{
		m_People[person.Handle] = person;
	}

	void PeopleStorage::SetPersonFileExtension(std::string_view extension)
	{
		m_PersonFileExtension = extension;
	}

	void PeopleStorage::SetWrapContentTolerance(size_t wrapContentTolerance)
	{
		m_WrapContentTolerance = wrapContentTolerance;
	}
}
