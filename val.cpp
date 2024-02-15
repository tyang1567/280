#include "val.h"

//Overloaded / operator
Value Value::operator/(const Value& op) const{
    if(GetType() == op.GetType()){
        if(IsInt() ){
            return Value(this->GetInt() / op.GetInt());
        }
        if(IsReal() ){
            return Value(this->GetReal() / op.GetReal());
        }
    }
    else if(IsInt() && op.IsReal()){
            return Value( (float)this->GetInt() / op.GetReal());
        }
    else if(IsReal() && op.IsInt()){
            return Value(this->GetReal() / (float) op.GetInt());
        }
    else if((op.IsInt() && op.GetInt() == 0 ) || (op.IsReal() && op.GetInt() == 0)){
        return Value();
    }
    return Value();
}

//Overloaded % operator
Value Value::operator%(const Value& oper) const{
    if(GetType() == oper.GetType() ){
        if(IsInt()){
            return Value (this->GetInt()%oper.GetInt());
        }
    }
    return Value();
}

//Overloaded == operator
Value Value::operator==(const Value& op) const {
    if(GetType() == op.GetType()){
        if(IsInt() ){
            return Value(this->GetInt() == op.GetInt());
        }
        if(IsString() ){
            return Value(this->GetString() == op.GetString());
        }
        if(IsBool()){
            return Value(this->GetBool() == op.GetBool());
        }
        if(IsReal()){
            return Value(this->GetReal() == op.GetReal());
        }
    }
    else if(IsInt() && op.IsReal()){
            return Value( (float)this->GetInt() == op.GetReal());
        }
    else if(IsReal() && op.IsInt()){
            return Value(this->GetReal() == (float) op.GetInt());
        }
        return Value();
    }

//Overloaded && operator
Value Value::operator&&(const Value& oper) const {
    if(GetType() == VBOOL && oper.GetType() == VBOOL){
        return Value(GetBool() == oper.GetBool());
    }
    return Value();
}

//Overloaded + operator
Value Value::operator+(const Value& op) const{
	if (GetType() == op.GetType()){
		if(IsInt() ){
			return Value(this->GetInt() + op.GetInt());
		}
		if(IsReal() ){
			return Value(this->GetReal() + op.GetReal());
		}
	}
	else if(IsInt() && op.IsReal() ){
		return Value( (float)this->GetInt() + op.GetReal());
	}
	else if(IsReal() && op.IsInt() ){
		return Value(this->GetReal() + (float)op.GetInt() );
	}
	return Value();
}

//Overloaded - operator
Value Value::operator- (const Value& op) const{
	if (GetType() == op.GetType()){
		if(IsInt() ){
			return Value(this->GetInt() - op.GetInt());
		}
		if(IsReal() ){
			return Value(this->GetReal() - op.GetReal());
		}
	}
	else if(IsInt() && op.IsReal() ){
		return Value( (float)this->GetInt() - op.GetReal());
	}
	else if(IsReal() && op.IsInt() ){
		return Value(this->GetReal() - (float)op.GetInt() );
	}
	return Value();
}

//Overloaded * operator
Value Value::operator* (const Value& op ) const{
	if (GetType() == op.GetType()){
		if(IsInt() ){
			return Value(this->GetInt() * op.GetInt());
		}
		if(IsReal() ){
			return Value(this->GetReal() * op.GetReal());
		}
	}
	else if(IsInt() && op.IsReal() ){
		return Value( (float)this->GetInt() * op.GetReal());
	}
	else if(IsReal() && op.IsInt() ){
		return Value(this->GetReal() * (float)op.GetInt() );
	}
	return Value();
}

//Integer division between Values
Value Value::div(const Value& op)const{
    if(IsReal() && op.IsReal()){
        return Value( (int)GetReal() / op.GetReal());
    }
    else if(IsInt() && op.IsInt()){
        return Value((int)GetInt() / op.GetInt());
    }
    else if(IsInt() && op.IsReal()){
        return Value((int)GetInt() / op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value((int)GetReal() / op.GetInt());
    }
    else if(op.GetInt() == 0 || op.GetReal() == 0)
        return Value();
    else{
        return Value();
    }
    }

//Overloaded > operator
Value Value::operator> (const Value& op) const {
	if(IsReal() && op.IsReal()){
		return Value(this->GetReal() > op.GetReal());
	}
	if(IsInt() && op.IsInt()){
		return Value(this->GetInt() > op.GetInt());
	}
	if(IsReal() && op.IsInt()){
		return Value(this->GetReal() > op.GetInt());
	}
	if(IsInt() && op.IsReal()){
		return Value(this->GetInt() > op.GetReal());
	}
	return Value();
}

//Overloaded < operator
Value Value::operator< (const Value& op) const{
	if(IsReal() && op.IsReal()){
		return Value(this->GetReal() < op.GetReal());
	}
	if(IsInt() && op.IsInt()){
		return Value(this->GetInt() < op.GetInt());
	}
	if(IsReal() && op.IsInt()){
		return Value(this->GetReal() < op.GetInt());
	}
	if(IsInt() && op.IsReal()){
		return Value(this->GetInt() < op.GetReal());
	}
	return Value();
}

//Overloaded idiv
Value Value::idiv(const Value& op) const{
    if(IsReal() && op.IsReal()){
    return Value( (int)GetReal() / op.GetReal());
    }
    else if(IsInt() && op.IsInt()){
        return Value((int)GetInt() / op.GetInt());
    }
    else if(IsInt() && op.IsReal()){
        return Value((int)GetInt() / op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value((int)GetReal() / op.GetInt());
    }
    else if(op.GetInt() == 0 || op.GetReal() == 0)
        return Value();
    else{
        return Value();
    }
}

//Overloaded || operator
Value Value::operator|| (const Value& oper) const{
	if(GetType() == VBOOL && oper.GetType() == VBOOL){
		return Value( this->GetBool() || oper.GetBool());
	}
	return Value();
}

//Overloaded ! operator
Value Value::operator! () const{
	if(IsBool()){
		return Value(!GetBool());
	}
	return Value();
}