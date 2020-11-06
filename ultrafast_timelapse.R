nb_seconds <- 10
nb_frames <- 25 * nb_seconds
basedir <- "/mnt/big/nick/cams"
noon_hour <- 13
args <- commandArgs(trailingOnly=TRUE)
if (!exists("indir")) indir <- args[1]
if (!exists("filtermode")) filtermode <- args[2] # full / summer
camID <- basename(indir)
outdir <- sprintf("tl_%s_%s", filtermode, camID)
writeLines(sprintf("Outdir: %s", outdir))
dir.create(outdir, showWarnings = FALSE)
cachefile <- sprintf(".images_%s.Rdata", camID)
if (file.exists(cachefile)) {
	load(cachefile)
} else {
	images <- data.frame(datei = list.files(indir, pattern="jpg"), stringsAsFactors=FALSE)
	images$pfad <- sprintf("%s/%s", indir, images$datei)
	images$filesize <- file.info(images$pfad)$size
	if (sum(grepl("_", images$datei)) > 0) {
		images$timestamp <- as.POSIXct(sub("netcam", "", images$datei), tz="", "%Y%m%d_%H%M%S")
		images$unixtime <- as.numeric(images$timestamp)
	} else {
		images$unixtime <- as.integer(sub(".jpg", "", images$datei))
		images$timestamp <- as.POSIXct(images$unixtime, origin="1970-01-01")
	}
	images$date <- as.Date(images$timestamp)
	images$stunde <- as.numeric(format(images$timestamp, "%H")) + as.numeric(format(images$timestamp, "%M")) / 60
	images$jahr <- as.numeric(format(images$timestamp, "%Y"))
	images$monat <- as.numeric(format(images$timestamp, "%m"))
	images$midiff <- abs(noon_hour - images$stunde)
	images <- images[images$date < Sys.Date(),] # Images from the future are not plausible
	save(images, file=cachefile)
}
if (filtermode == "summer") subimg <- images[images$monat %in% 4:10 & images$midiff < 2,]
if (filtermode == "noon") subimg <- images[images$midiff < 2,]
if (filtermode == "none") subimg <- images
writeLines(sprintf("images = %d, days = %d, filtered_images = %d, filtered_days = %d", nrow(images), length(unique(images$date)), nrow(subimg), length(unique(subimg$date))))
days <- sort(unique(subimg$date))
nb_days <- length(days)

# Determine number of blocks

suitable_divisors <- function(number) {
	daydiv <- numbers::divisors(nb_days)
	daydiv[daydiv > 2 & daydiv < 30]
}

daydiv <- suitable_divisors(nb_days)
while (length(daydiv) == 0) {
	nb_days <- nb_days - 1
	daydiv <- suitable_divisors(nb_days)
}
daydiv <- max(daydiv)
if (nb_days != length(days)) stop("todo: remove days with least number of images")
nb_blocks <- nb_days / daydiv

# Determine number of images per block
imgs_per_block <- round(nb_frames / nb_blocks)
if (imgs_per_block < 1) imgs_per_block <- 1
nb_frames_new <- nb_blocks * imgs_per_block
writeLines(sprintf("nb_days = %d, daydiv = %d, nb_blocks = %d, nb_frames = %d, nb_frames_new = %d, imgs_per_block = %d", nb_days, daydiv, nb_blocks, nb_frames, nb_frames_new, imgs_per_block))

# Select largest image in first block as initial frame.
subimg <- subimg[order(subimg$unixtime),]
seldays <- days[(0*daydiv+1):(0*daydiv+daydiv)]
selimgs <- subimg[subimg$date %in% seldays,]
selimgs <- selimgs[order(selimgs$filesize, decreasing=TRUE),]
selimgs <- selimgs[selimgs$filesize >= mean(selimgs$filesize),] # optional
init_img <- selimgs[1, "pfad"]
subimg[subimg$pfad == init_img, "Auswahl"] <- TRUE
for (i in 2:nrow(selimgs)) {
	cmd <- sprintf("image_histogram_distance_rgb.py %s %s", init_img, selimgs[i, "pfad"])
	d <- as.numeric(system(cmd, intern=TRUE))
	if (length(d) == 1) selimgs[i, "distance"] <- d
	writeLines(sprintf("%d/%d: %f", i, nrow(selimgs), selimgs[i, "distance"]))
}

# Select n with lowest distance to initial frame.
selimgs <- selimgs[order(selimgs$distance),]
top_imgs <- selimgs[1:(imgs_per_block-1), "unixtime"]
subimg[subimg$unixtime %in% top_imgs, "Auswahl"] <- TRUE
last_img <- tail(subimg[!is.na(subimg$Auswahl), "pfad"], n=1)

# Copy files
for (filename in subimg[subimg$Auswahl == TRUE, "datei"]) {
	f1 <- sprintf("%s/%s", indir, filename)
	f2 <- sprintf("%s/%s", outdir, filename)
	if (!file.exists(f2)) file.copy(f1, f2)
}

# Loop:	Select n with lowest distance to sequentially last selected of previous block.
for (packi in 1:(nb_blocks-1)) {
	seldays <- days[(packi*daydiv+1):(packi*daydiv+daydiv)]
	selimgs <- subimg[subimg$date %in% seldays,]
	selimgs <- selimgs[selimgs$filesize >= mean(selimgs$filesize),] # optional
	if (sum(selimgs$Auswahl, na.rm=TRUE) > 0) {
		last_img <- tail(selimgs[!is.na(selimgs$Auswahl), "pfad"], n=1)
		next
	}
	for (i in 1:nrow(selimgs)) {
		cmd <- sprintf("image_histogram_distance_rgb.py %s %s", last_img, selimgs[i, "pfad"])
		d <- as.numeric(system(cmd, intern=TRUE))
		if (length(d) == 1) selimgs[i, "distance"] <- d
		writeLines(sprintf("%d/%d -> %d/%d: %f", packi, nb_blocks-1, i, nrow(selimgs), selimgs[i, "distance"]))
	}
	selimgs <- selimgs[order(selimgs$distance),]
	top_imgs <- selimgs[1:imgs_per_block, "unixtime"]
	subimg[subimg$unixtime %in% top_imgs, "Auswahl"] <- TRUE
	last_img <- tail(subimg[!is.na(subimg$Auswahl), "pfad"], n=1)
	# Copy files
	for (filename in subimg[subimg$Auswahl == TRUE, "datei"]) {
		f1 <- sprintf("%s/%s", indir, filename)
		f2 <- sprintf("%s/%s", outdir, filename)
		if (!file.exists(f2)) file.copy(f1, f2)
	}
}

cmd <- sprintf("median_images.py %s 3", outdir)
writeLines(cmd)
system(cmd)

cmd <- sprintf("date_to_timelapse_monyear.py %s_median3", outdir)
writeLines(cmd)
system(cmd)

cmd <- sprintf("ffmpeg -y -framerate 25 -pattern_type glob -i '%s_median3/*.jpg' -codec:v libx264 -crf 18 -profile:v main %s.mp4", outdir, outdir)
writeLines(cmd)
system(cmd)

# 1280x960 -> 960x540: 320 420 1280 960
# crop_images.py /mnt/big/nick/cams/lauterbrunnen 320 420 1280 960
# crop_images.py /mnt/big/nick/cams/scheidegg 160 150 1120 690
