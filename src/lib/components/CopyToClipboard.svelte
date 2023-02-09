<script lang="ts">
	export let text: string;
	const copy = () => {
		navigator.clipboard.writeText(text);
	};
</script>

<button on:click={copy} title="Click to copy">{text}</button>
