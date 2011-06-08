function deleteMovies(doc) {
  if (!confirm("Remove " + files.length + " files?")) {
    return;
  }

  for (i = 0; i < files.length; i++) {
    var file = files[i];
    ajax_add_request();
    new Ajax.Request('/mythweb/tv/recorded',
                     {
                       method: 'post',
                       onSuccess: http_success,
                       onFailue: http_failure,
                       parameters: { ajax:       'yes',
                                     'delete':   'yes',
                                     chanid:     file.chanid,
                                     starttime:  file.starttime,
                                     forget_old: 'no',
                                     id:         i,
                                     file:       Object.toJSON(file)
                                   }
                     });
  }
}

