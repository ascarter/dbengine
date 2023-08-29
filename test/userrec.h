#ifndef __USERREC_H__
#define __USERREC_H__

struct UserRecord : public DbRecord
{
	UINT	UserId;
	char	FirstName[20];
	char	LastName[20];
	char	Login[20];
	UINT	Age;
	UINT	BirthMonth;
	UINT	BirthDay;
	UINT	BirthYear;
	UINT	Uniform;
	char	Hometown[50];
	char	Acquired[50];
};


#endif // __USERREC_H__


