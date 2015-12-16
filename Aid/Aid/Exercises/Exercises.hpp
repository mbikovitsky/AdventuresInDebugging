#pragma once

#include <list>
#include <boost\uuid\uuid.hpp>


namespace Aid { namespace Exercises {


struct Exercise
{
	boost::uuids::uuid id;
	std::wstring name;
	std::function<void()> implementation;
};

class ExerciseExecutor final
{
public:
    ExerciseExecutor(std::initializer_list<const Exercise> exercises);
    ExerciseExecutor(const ExerciseExecutor &other) = delete;
    ExerciseExecutor & operator=(const ExerciseExecutor &other) = delete;
    ~ExerciseExecutor() = default;

    void Seek();
    void Seek(const boost::uuids::uuid &id);

    void Load(const std::wstring &fileName);
    void Save(const std::wstring &fileName);

    bool ExecuteOne();

private:
    typedef std::list<const Exercise> ListType;

    const ListType m_exercises;
    ListType::const_iterator m_currentExercise;
};


extern ExerciseExecutor g_exercises;


} }
