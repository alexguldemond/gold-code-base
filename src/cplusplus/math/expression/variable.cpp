#include "Gold/math/variable.hpp"
#include <stdexcept>

namespace Gold {
    namespace math {

	variable::variable() { 
	    //Intentionally empty
	}

	variable::variable(const std::string& str) : expression(_check_variable(str)) { }

	variable::variable(const variable& other) : expression(other) { }

	variable::variable(variable&& other) : expression(other) { }

	variable& variable::operator=(const variable& other) {
	    if (&other != this) {
		expression::operator=(other);
	    }
	    return *this;
	}

	variable& variable::operator=(variable&& other) {
	    if (&other != this) {
		expression::operator=(other);
	    }
	    expression::operator=(other);
	    return *this;
	}

	std::string variable::_check_variable(const std::string& str) {
	    if (!utils::is_string_var(str)) {
		throw std::invalid_argument(std::string(str).append(" can't be used as a variable"));
	    }
	    return str;
	}

    }
}
