/**
 * @file robotFile.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Ecriture et lecture dans un fichier nornal ou compresse
 * Pour compiler, linker avec la lib dynamic libz.so:  -lz
 * Pour ne pas utiliser la libz, compiler les fichiers avec le define 
 * FILE_NO_ZLIB
 */

#ifndef __FILE_H__
#define __FILE_H__

#include "robotTypes.h"

typedef enum FileMode {
    FILE_MODE_READ,
    FILE_MODE_WRITE
} FileMode;

/**
 * @class File
 * Fichier normal, fonctions d'ecritures et de lectures
 */
class File
{
 public:
    /** @brief Constructeur qui ouvre le fichier */
    File(const char* path, FileMode mode);
    /** @brief Constructeur de base */
    File();
    /** @brief Destrcuteur qui ferme le fichier */
    virtual ~File();

    /** @brief Ouvre un fichier et retourne true si tout s'est bien passe */
    virtual bool open(const char* path, FileMode mode);
    /** @brief Ferme le fichier s'il est ouvert */
    virtual void close();
    /** @brief Lit dans le fichier. Retourne le nombre d'octects lus */
    virtual int  read(Byte*  data, unsigned int buflength);
    /** @brief Ecrit dans le fichier. Retourne le nombre d'octects ecris */
    virtual int  write(Byte* data, unsigned int buflength);
    
 private:
    FILE* fptr_;
};

#ifndef FILE_NO_ZLIB

#include "zlib.h"

/**
 * @class ZFile
 * Fichier compresse (.gz), fonctions d'ecritures et de lectures
 */
class ZFile : public File
{
 public:
    /** @brief Constructeur qui ouvre le fichier */
    ZFile(const char* path, FileMode mode);
    /** @brief Constructeur de base */
    ZFile();
    /** @brief Destrcuteur qui ferme le fichier */
    ~ZFile();

    /** @brief Ouvre un fichier et retourne true si tout s'est bien passe */
    bool open(const char* path, FileMode mode);
    /** @brief Ferme le fichier s'il est ouvert */
    void close();
    /** @brief Lit dans le fichier. Retourne le nombre d'octects lus */
    int  read(Byte*  data, unsigned int buflength);
    /** @brief Ecrit dans le fichier. Retourne le nombre d'octects ecris */
    int  write(Byte* data, unsigned int buflength);
    
 private:
    gzFile file_;
};
#endif // FILE_NO_ZLIB


// ----------------------------------------------------------------------------
// File::File
// ----------------------------------------------------------------------------
inline File::File(const char* path, FileMode mode) :
    fptr_(NULL)
{
    open(path, mode);
}

// ----------------------------------------------------------------------------
// File::File
// ----------------------------------------------------------------------------
inline File::File() :
    fptr_(NULL)
{
}

// ----------------------------------------------------------------------------
// File::File
// ----------------------------------------------------------------------------
inline File::~File()
{
    close();
}

// ----------------------------------------------------------------------------
// File::open
// ----------------------------------------------------------------------------
inline bool File::open(const char* path, FileMode mode)
{
    fptr_ = fopen(path, (mode==FILE_MODE_READ)?"rb":"wb");
    return fptr_ != NULL;
}

// ----------------------------------------------------------------------------
// File::close
// ----------------------------------------------------------------------------
inline void File::close()
{
    if (fptr_) {
        fclose(fptr_);
        fptr_ = NULL;
    }
}

// ----------------------------------------------------------------------------
// File::read
// ----------------------------------------------------------------------------
inline int File::read(Byte*  data, unsigned int buflength)
{
    if (!fptr_) return -1;
    return fread(data, 1, buflength, fptr_);
}

// ----------------------------------------------------------------------------
// File::write
// ----------------------------------------------------------------------------
inline int File::write(Byte* data, unsigned int buflength)
{
    if (!fptr_) return -1;
    return fwrite(data, 1, buflength, fptr_);
}

#ifndef FILE_NO_ZLIB


// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline ZFile::ZFile(const char* path, FileMode mode) :
    File(), file_(NULL)
{
    open(path, mode);
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline ZFile::ZFile() :
    File(), file_(NULL)
{
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline ZFile::~ZFile()
{
    close();
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline bool ZFile::open(const char* path, FileMode mode)
{
    char path2[256];
    if (strcmp(path+max((int)strlen(path)-3, 0), ".gz") != 0) {
        snprintf(path2, 256, "%s.gz", path);
    } else {
      strcpy(path2, path);
    }
    file_ = gzopen(path2, (mode==FILE_MODE_READ)?"rb":"wb");
    return file_ != NULL;
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline void ZFile::close()
{
    if (file_) {
        gzclose(file_);
        file_ = NULL;
    }
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline int ZFile::read(Byte*  data, unsigned int buflength)
{
    if (!file_) return -1;
    return gzread(file_, data, buflength);
}

// ----------------------------------------------------------------------------
// ZFile::ZFile
// ----------------------------------------------------------------------------
inline int ZFile::write(Byte* data, unsigned int buflength)
{
    if (!file_) return -1;
    return gzwrite(file_, data, buflength);
}
#endif // FILE_NO_ZLIB


#endif // __FILE_H__
