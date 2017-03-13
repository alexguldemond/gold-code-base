#include "Gold/new_math/simplify/simplify.hpp"

#include <algorithm>
#include <cmath>

namespace Gold {
    namespace math {
	namespace simplify {

	    void simplify_rational_number(utils::node_ptr root) {
		if (utils::kind(root) == utils::FRACTION) {
		    utils::node_ptr numerator = root->children[0];
		    utils::node_ptr denominator;
		    if (root->token == "/") {
			denominator = root->children[1];
		    } else {
			denominator = root->children[1]->children[0];
		    }
	
		    int num = std::stoi(numerator->token);
		    int den = std::stoi(denominator->token);
		    
		    if (den == 0) {
			root->token = "";
			root->children.clear();
		    }
		    
		    std::pair<int, int> pair = utils::simplify_rational_number(num, den);
		    num = pair.first;
		    den = pair.second;
		    
		    if (den != 1) {
			numerator->token = std::to_string(num);
			denominator->token = std::to_string(den);
		    } else {
			root->token = std::to_string(num);
			root->children.clear();
		    } 
		}
	    }

	    void simplify_rational_tree(utils::node_ptr root) {
		if (!root || utils::is_leaf(root)) {
		    return;
		}

		for (auto iter = root->children.begin(); iter != root->children.end(); iter++) {
		    simplify_rational_tree(*iter);
		    if (utils::kind(*iter) == utils::UNDEFINED) {
			root->token = "";
			root->children.clear();
			return;
		    }
		}

		/*		std::map<int, utils::node_vec> kind_map;
		for (auto iter = root->children.begin(); iter != root->children.end(); iter++) {
		    int kind = utils::kind(*iter);
		    if (kind_map.find(kind) == kind_map.end()) {
			kind_map.insert( std::pair<int, utils::node_vec>(kind, { *iter }));
		    } else {
			kind_map[kind].push_back(*iter);
		    }
		}
		
		root->children.clear();

		for (auto iter = kind_map.begin(); iter != kind_map.end(); iter++) {
		    root->children.insert(root->children.end(), iter->begin(), iter->end());
		    }*/

		int kind = utils::kind(root);
		switch (kind) {
		case utils::ADD:
		    _simplify_add(root);
		    break;
		case utils::SUBTRACT:
		    _simplify_subtract(root);
		    break;
		case utils::MULTIPLY:
		    _simplify_multiply(root);
		    break;
		case utils::DIVIDE:
		    _simplify_divide(root);
		    break;
		case utils::POWER:
		    _simplify_power(root);
		    break;
		}
		
		if (root->token == "+" || root->token == "*") {
		    std::sort(root->children.begin(), root->children.end(), utils::node_compare);
		}
		simplify_rational_number(root);
	    }

	    void _simplify_add(utils::node_ptr root) { }

	    void _simplify_subtract(utils::node_ptr root) { }

	    void _simplify_multiply(utils::node_ptr root) { }

	    void _simplify_divide(utils::node_ptr root) { }

	    void _simplify_power(utils::node_ptr root) {
		if (utils::kind(root) != utils::POWER || root->children.size() != 2) {
		    return;
		}
		
		utils::node_ptr base = root->children[0];
		utils::node_ptr expo = root->children[1];

		if (base->token == "0" && expo->token == "0") {
		    root->token = "";
		    root->children.clear();
		    return;
		}
		else if (base->token == "0") {
		    root->token = "0";
		    root->children.clear();
		    return;
		}
		else if (base->token == "1" || expo->token == "0") {
		    root->token = "1";
		    root->children.clear();
		    return;
		}
		else if ( expo->token == "1") {
		    root->token = base->token;
		    root->children = base->children;
		    return;
		}
		else if (expo->token == "-1") {
		    return;
		}
		
		int base_kind = utils::kind(base);
		int expo_kind = utils::kind(expo);

		if (base_kind == utils::INTEGER && expo_kind == utils::INTEGER) {
		    int b = std::stoi(base->token);
		    int e = std::stoi(expo->token);
		    if ( e > 0 ) {
			int result = pow(b,e);
			root->token = std::to_string(result);
			root->children.clear();
			return;
		    }
		    else if ( e < 0 ) {
			int result = pow(b, -e);
			root->token = "^";
			
			utils::node_ptr left = std::make_shared<utils::node>();
			left->token = std::to_string(result);
			
			utils::node_ptr right = std::make_shared<utils::node>();
			right->token = "-1";
			
			root->children = { left, right};
			return;
		    }
		    else {
			root->token = "1";
			root->children.clear();
			return;
		    }
		}
		//Either one of both are doubles
		else if ( (base_kind == utils::NUMBER && expo_kind == utils::NUMBER) || 
			  (base_kind == utils::NUMBER && expo_kind == utils::INTEGER) || 
			  (base_kind == utils::INTEGER && expo_kind == utils::NUMBER) ) {
		    double b = std::stof(base->token);
		    double e = std::stof(expo->token);
		    double result = pow(b,e);
		    root->token = std::to_string(result);
		    root->children.clear();
		    return;
		}
		//If the base is a negative number
		else if (base_kind == utils::MULTIPLY && base->children.size() == 2 && base->children[0]->token == "-1" && utils::is_leaf(base->children[1])) {
		    base = base->children[1];
		    base_kind = utils::kind(base);
		    if (base_kind == utils::INTEGER && expo_kind == utils::INTEGER) {
			int b = -std::stoi(base->token);
			int e = std::stoi(expo->token);
			if ( e > 0 ) {
			    int result = pow(b,e);
			    if (result < 0) {
				root->token = "*";
				root->children = { std::make_shared<utils::node>(), std::make_shared<utils::node>() };
				root->children[0]->token = "-1";
				root->children[1]->token = std::to_string(-result);
			    }
			    else {
				root->token = std::to_string(result);
				root->children.clear();
			    }
			    return;
			}
			else if ( e < 0 ) {
			    int result = pow(b, -e);
			    root->token = "^";
			    
			    utils::node_ptr left = std::make_shared<utils::node>();
			    if (result < 0) {
				left->token = "*";
				left->children = { std::make_shared<utils::node>(), std::make_shared<utils::node>() };
				left->children[0]->token = "-1";
				left->children[1]->token = std::to_string(-result);
			    }
			    else {
				left->token = std::to_string(result);
				left->children.clear();
			    }
			    
			    utils::node_ptr right = std::make_shared<utils::node>();
			    right->token = "-1";
			    
			    root->children = { left, right};
			    return;
			}
			else {
			    root->token = "1";
			    root->children.clear();
			    return;
			}
		    }
		    //Either one of both are doubles
		    else if ( (base_kind == utils::NUMBER && expo_kind == utils::NUMBER) || 
			      (base_kind == utils::NUMBER && expo_kind == utils::INTEGER) || 
			      (base_kind == utils::INTEGER && expo_kind == utils::NUMBER) ) {
			double b = std::stof(base->token);
			double e = std::stof(expo->token);
			double result = pow(b,e);
			root->token = std::to_string(result);
			root->children.clear();
			return;
			}
		    
		    }
	    }
		  
	}
    }
}	