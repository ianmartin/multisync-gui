#ifndef MSYNC_SUPPORT_H_
#define MSYNC_SUPPORT_H_

void msync_error_message(GtkWindow* window, gboolean threadsafe, char *format, ...);
const char *OSyncChangeType2String(OSyncChangeType c);

#endif /*MSYNC_SUPPORT_H_*/
