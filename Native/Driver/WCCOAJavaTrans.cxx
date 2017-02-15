// Our transformation class PVSS <--> Hardware
#include <WCCOAJavaTrans.hxx>
#include <WCCOAJavaDrv.hxx>
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>    // Our Variable type Basics/Variables
#include <IntegerVar.hxx>

#include <stdio.h>

//----------------------------------------------------------------------------


WCCOAJavaTrans::WCCOAJavaTrans(const CharString& name, TransformationType type)
{
	WCCOAJavaTrans::name = CharString(name);
	WCCOAJavaTrans::type = type;
	//std::cout << "newTrans" << std::endl;
	jTransObject = WCCOAJavaDrv::thisManager->JavaTransformationNewObject(this, name, type);
	if (jTransObject == NULL) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			WCCOAJavaDrv::ManagerName, "WCCOAJavaTrans::WCCOAJavaTrans", CharString("no transformation object!"));

	}
}

WCCOAJavaTrans::~WCCOAJavaTrans()
{
	//std::cout << "delTrans" << std::endl;
	WCCOAJavaDrv::thisManager->JavaTransformationDelObject(jTransObject);
}


//----------------------------------------------------------------------------

TransformationType WCCOAJavaTrans::isA() const
{
	return type;
}

//----------------------------------------------------------------------------

TransformationType WCCOAJavaTrans::isA(TransformationType type) const
{
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

//----------------------------------------------------------------------------

Transformation *WCCOAJavaTrans::clone() const
{
	return new WCCOAJavaTrans(name, type);
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int WCCOAJavaTrans::itemSize() const
{
	if (jTransObject == NULL) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			WCCOAJavaDrv::ManagerName, "WCCOAJavaTrans::itemSize()", CharString("no transformation object!"));
		return 0;
	}
	else {
		return WCCOAJavaDrv::thisManager->JavaTransformationGetSize(jTransObject);
	}
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType WCCOAJavaTrans::getVariableType() const
{
	if (jTransObject == NULL) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			WCCOAJavaDrv::ManagerName, "WCCOAJavaTrans::getVariableType()", CharString("no transformation object!"));
		return VARIABLE;
	}
	else {
		return WCCOAJavaDrv::thisManager->JavaTransformationGetVariableType(jTransObject);
	}

}

//----------------------------------------------------------------------------
// Convert data from PVSS to Hardware.
// In this example the subindex is ignored

PVSSboolean WCCOAJavaTrans::toPeriph(PVSSchar *buffer, PVSSuint len,
	const Variable &var, const PVSSuint subix) const
{
	// TODO  everything in here has to be adapted to your needs

	//// Be paranoic, check variable type
	//if (var.isA() != INTEGER_VAR)
	//{
	//	// Throw error message
	//	ErrHdl::error(
	//		ErrClass::PRIO_SEVERE,             // Data will be lost
	//		ErrClass::ERR_PARAM,               // Wrong parametrization
	//		ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
	//		"WCCOAJavaTrans", "toPeriph",     // File and function name
	//		"Wrong variable type for data"     // Unfortunately we don't know which DP
	//		);

	//	return PVSS_FALSE;
	//}

	//// Check data len. TextVar::getString returns a CharString
	//if (len < static_cast<const TextVar &>(var).getString().len() + 1)
	//{
	//	// Throw error message
	//	ErrHdl::error(
	//		ErrClass::PRIO_SEVERE,             // Data will be lost
	//		ErrClass::ERR_IMPL,                // Mus be implementation
	//		ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
	//		"WCCOAJavaTrans::toPeriph",       // File and function name
	//		"Data buffer too small; need:" +
	//		CharString(static_cast<const TextVar &>(var).getString().len() + 1) +
	//		" have:" + CharString(len)
	//		);

	//	return PVSS_FALSE;
	//}

	//sprintf(reinterpret_cast<char *>(buffer), "%s", static_cast<const TextVar &>(var).getValue());
	if (jTransObject == NULL) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			WCCOAJavaDrv::ManagerName, "WCCOAJavaTrans::itemSize()", CharString("no transformation object!"));
		return PVSS_FALSE;
	}
	else {
		return WCCOAJavaDrv::thisManager->JavaTransformationToPeriph(jTransObject, buffer, len, var, subix);
	}
}

//----------------------------------------------------------------------------
// Conversion from Hardware to PVSS

VariablePtr WCCOAJavaTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen,
	const PVSSuint subix ) const
{
	// TODO  everything in here has to be adapted to your needs

	// Return pointer to new PVSS Variable
	//Variable *var = Variable::allocate(varType);
	//(*var) = TextVar(reinterpret_cast<const char *>(buffer));   // virtual operator= in all Variables
	if (jTransObject == NULL) {
		ErrHdl::error(ErrClass::PRIO_SEVERE, ErrClass::ERR_IMPL, ErrClass::UNEXPECTEDSTATE,
			WCCOAJavaDrv::ManagerName, "WCCOAJavaTrans::itemSize()", CharString("no transformation object!"));
		return NULL;
	}
	else {
		return WCCOAJavaDrv::thisManager->JavaTransformationToVar(jTransObject, buffer, dlen, subix);
	}	
}

//----------------------------------------------------------------------------
