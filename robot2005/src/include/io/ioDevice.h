/**
 * @file ioDevice.h
 *
 * @author Florian Loitsch
 *
 * abstract class for (non-)blocking IO.
 */

#pragma once

#include "robotBase.h"
#include <vector>

// TODO: this file should be replaced by robotTypes.h
#include "types.h"

// forward declaration
class IoDevice;

typedef std::vector<IoDevice*> IoDeviceVector;

typedef void (*NonBlockingIoCallback) (void* userdata,
				       const IoByte* data,
				       int length);

class IoDevice : public RobotBase {
public:
    IoDevice();
    IoDevice(const char* name, ClassId id);
    virtual ~IoDevice();

public: // methods
    /**
     * @brief Opens the port.
     * Necessary information must either be passed during construction, or
     * using setter-functions.
     */
    virtual bool open() = 0;

    /**
     * @brief true, if open.
     */
    virtual bool isOpen() const = 0;

    /**
     * @brief Closes the port.
     * A device can't be used once it's closed.
     */
    virtual bool close() = 0;

    /**
     * @brief Resets the port.
     * Depends heavily on the underlying device. Might not do anything.
     */
    virtual bool reset() = 0;

public: // blocking read/write methods

    // TODO: setTimeOut [flo]. quite important actually.

    /** 
     * @brief Reads 1 byte
     * Returns true if successful, false otherwise.
     */
    virtual bool read(IoByte* buf) {
	unsigned int length = 1;
	return read(buf, length);
    }

    /** 
     * @brief Reads 'length' bytes
     * Returns true if successful, false otherwise. If any case
     * 'length' is updated to contain the number of read bytes.
     * TODO: i would prefer passing changing parameters only as pointers.
     */
    virtual bool read(IoByte* buf, unsigned int& length) = 0;
    
    /** 
     * @brief Writes 1 byte.
     * Returns true if successful, false otherwise
     */
    virtual bool write(IoByte buf) {
	unsigned int length = 1;
	return write(&buf, length);
    }
    
    /**
     * @brief Writes 'length' bytes
     * Returns true if successful, false otherwise. In any case
     * 'length' is update to contain the number of sent bytes.
     * TODO: i would prefer passing changing parameters only as pointers.
     */
    virtual bool write(IoByte* buf, unsigned int& length) = 0;

    /**
     * @brief combines write and read.
     * If the send contains a request to which the ioDevice must answer,
     * this method should be used, as the communication is guaranteed to
     * be uninterrupted.
     */
    virtual bool writeRead(IoByte* sendBuffer, unsigned int& sendLength,
			   IoByte* receiveBuffer, unsigned int& receiveLength)
	{
	    return write(sendBuffer, sendLength)
		&& read(receiveBuffer, receiveLength);
	}

    /**
     * @brief writes 1 byte, and receives 1 byte in one "transaction".
     * @see writeRead(IoByte*, unsigned int&, IoByte*, unsigned int&)
     */
    virtual bool writeRead(IoByte send, IoByte* receive) {
	unsigned int sendLength = 1;
	unsigned int receiveLength = 1;
	return writeRead(&send, sendLength, receive, receiveLength);
    }
    
    /**
     * @brief writes 1 byte, and receives 'length' bytes in one "transaction".
     * @see writeRead(IoByte*, unsigned int&, IoByte*, unsigned int&)
     */
    virtual bool writeRead(IoByte send,
			   IoByte* receive, unsigned int& receiveLength) {
	unsigned int sendLength = 1;
	return writeRead(&send, sendLength, receive, receiveLength);
    }

    /**
     * @brief writes 'length' bytes, and receives 1 byte in one "transaction".
     * @see writeRead(IoByte*, unsigned int&, IoByte*, unsigned int&)
     */
    virtual bool writeRead(IoByte* send, unsigned int& sendLength,
			   IoByte* receive) {
	unsigned int receiveLength = 1;
	return writeRead(send, sendLength, receive, receiveLength);
    }

public: // non-blocking read/write methods

    /**
     * @brief returns true, if this device can do non-blocking operations.
     */
    virtual bool canListen() const { return false; }
    
    /**
     * @brief starts listening (in a non-blocking way)
     * received data is forwarded to the registered callback-function.
     * It is not allowed to use the non-blocking methods,
     * if the port is in listening-mode. TODO: is this really true? [flo]
     */
    virtual void startListening() { /* DO NOTHING */ }

    /**
     * @brief stops listening
     */
    virtual void stopListening() { /* DO NOTHING */ }

    /**
     * @brief registers a callback-function which is executed for each
     * new data that has been received (in listening mode).
     * be aware, that this method could potentially execute at
     * any moment, and take appropriate precautions (Locks...)
     * Only one callback-function exists. If a new one is registered, the
     * old one is lost.
     * Usually implementers don't need to reimplement this method.
     */
    void registerCallback(NonBlockingIoCallback cb, void* userData);

    /**
     * @brief unregisters a callback-function.
     * If no callback is registered, nothing happens.
     * Usually implementers don't need to reimplement this method.
     */
    void unregisterCallback();

protected:
    /**
     * @brief calls the callback-function.
     * convenience-method, so implementers don't need to know anything about
     * the callback.
     */
    void signalNewData(const IoByte* data, int length);

private:
    NonBlockingIoCallback callback_;
    void* userData_;
};

class IoDeviceOpenerCloser {
public:
    IoDeviceOpenerCloser(IoDevice* device)
	: device_(device)
	{
	    device->open();
	}

    ~IoDeviceOpenerCloser()
	{
	    device_->close();
	}

    void earlyClose()
	{
	    device_->close();
	}
private:
    IoDevice* device_;
};
